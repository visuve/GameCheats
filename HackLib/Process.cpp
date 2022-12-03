#include "Process.hpp"
#include "ByteStream.hpp"
#include "OpCodes.hpp"
#include "Process.hpp"
#include "SHA256.hpp"
#include "StrConvert.hpp"
#include "System.hpp"
#include "Win32Thread.hpp"
#include "Win32Event.hpp"
#include "TypeHelp.hpp"

Process::Process(DWORD pid) :
	_pid(pid),
	_baseAddress(System::ModuleEntryByPid(pid).modBaseAddr),
	_targetProcess(PROCESS_ALL_ACCESS, pid)
{
	if (!_targetProcess)
	{
		throw Win32Exception("OpenProcess");
	}

	Log << "Process" << pid << "opened";
}

Process::~Process()
{
	_threads.clear();
	_regions.clear();
	_targetProcess.Reset();
}

void Process::WaitForIdle()
{
	DWORD result = 0;

	do
	{
		result = _targetProcess.WaitForInputIdle(1000ms);

	} while (result == WAIT_TIMEOUT);

	if (result != 0)
	{
		throw Win32Exception("WaitForInputIdle", result);
	}
}

bool Process::Verify(std::string_view expectedSHA256) const
{
	// We could grab the path from ModuleEntryByPid right in the beginning,
	// but it's MAX_PATH limited and ANSI, hence unreliable.
	const std::filesystem::path path = _targetProcess.Path();
	return SHA256(path) == expectedSHA256;
}

MODULEENTRY32W Process::FindModuleEntry(std::wstring_view name) const
{
	return System::ModuleEntryByName(_pid, name);
}

IMAGE_NT_HEADERS Process::NtHeader() const
{
	const auto dosHeader = Read<IMAGE_DOS_HEADER>(_baseAddress);

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
	{
		throw LogicException("Invalid DOS header");
	}

	const auto ntHeaders = Read<IMAGE_NT_HEADERS>(_baseAddress + dosHeader.e_lfanew);

	if (ntHeaders.Signature != IMAGE_NT_SIGNATURE) 
	{
		throw LogicException("Invalid NT headers");
	}

#ifdef _WIN64
	if (ntHeaders.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
#else
	if (ntHeaders.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
#endif
	{
		throw LogicException("Invalid magic");
	}

	if (!(ntHeaders.FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE))
	{	
		throw LogicException("Not an executable");
	}

	return ntHeaders;
}

IMAGE_IMPORT_DESCRIPTOR Process::FindImportDescriptor(std::string_view moduleName) const
{
	IMAGE_NT_HEADERS ntHeader = NtHeader();

	IMAGE_DATA_DIRECTORY importEntry = ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	Pointer importDescriptorPtr = Address(importEntry.VirtualAddress);

	// https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-imagervatova

	IMAGE_IMPORT_DESCRIPTOR iid = {};
	std::string buffer(MAX_PATH , '\0');

	do
	{
		iid = Read<IMAGE_IMPORT_DESCRIPTOR>(importDescriptorPtr);

		if (!iid.Characteristics)
		{
			break;
		}

		Read(Address(iid.Name), buffer.data(), buffer.size());

		// Cast to .c_str() because the buffer contains trailing nulls
		if (StrConvert::IEquals(moduleName, buffer.c_str()))
		{
			return iid;
		}

		importDescriptorPtr += sizeof(IMAGE_IMPORT_DESCRIPTOR);

	} while (iid.Characteristics);

	throw RangeException("Import not found");
}

Pointer Process::FindImportEntry(IMAGE_IMPORT_DESCRIPTOR iid, std::string_view functionName) const
{
	Pointer thunkPtr = Address(iid.OriginalFirstThunk);
	std::string buffer(MAX_PATH, '\0');

	const Pointer offset = thunkPtr - iid.FirstThunk;
	IMAGE_THUNK_DATA thunk = {};

	do
	{
		thunk = Read<IMAGE_THUNK_DATA>(thunkPtr);

		if (!thunk.u1.Function)
		{
			break;
		}

		Read(Address(thunk.u1.AddressOfData + 2), buffer.data(), buffer.size());

		// Cast to .c_str() because the buffer contains trailing nulls
		if (StrConvert::IEquals(functionName, buffer.c_str()))
		{
			return thunkPtr - offset;
		}

		thunkPtr += sizeof(IMAGE_THUNK_DATA);

	} while (thunk.u1.Function);


	throw RangeException("Function not found");
}

Pointer Process::FindImportAddress(std::string_view moduleName, std::string_view functionName) const
{
	IMAGE_IMPORT_DESCRIPTOR iid = FindImportDescriptor(moduleName);
	return _baseAddress + FindImportEntry(iid, functionName);
}

Pointer Process::AllocateMemory(size_t size)
{
	auto result = _regions.emplace(_targetProcess, size);

	if (!result.second)
	{
		throw RuntimeException("Catastrophic failure, pointer already existed");
	}

	MEMORY_BASIC_INFORMATION info = result.first->Query();

	Log << "Allocated" << info.RegionSize << "bytes at" << result.first->Address() ;

	return result.first->Address();
}

MemoryRegion Process::AllocateRegion(const std::initializer_list<MemoryRegion::NameTypePair>& pairs)
{
	size_t regionSize = 0;

	for (const auto& [name, type] : pairs)
	{
		size_t typeSize = SizeOfBasicType(type);

		if (typeSize == 0)
		{
			throw ArgumentException(name + " has unknown type");
		}

		regionSize += typeSize;
	}

	const Pointer region = AllocateMemory(regionSize);
	return MemoryRegion(region, pairs);
}

DWORD Process::SpawnThread(Pointer address, Pointer parameter, bool detached)
{
	HANDLE bare = _targetProcess.CreateRemoteThread(address, parameter);

	if (detached)
	{
		auto result = _threads.emplace(bare);

		if (!result.second)
		{
			throw RuntimeException("Catastrophic failure, pointer already existed");
		}

		return 0;
	}

	Win32Thread thread(bare);

	thread.Wait();

	return thread.ExitCode();
}

DWORD Process::InjectLibrary(const std::filesystem::path& path)
{
	const std::wstring str = path.wstring();
	const size_t bytes = str.size() * sizeof(wchar_t);

	Pointer namePtr = AllocateMemory(bytes + 1); // +1 to include null terminator
	Write(namePtr, str.data(), bytes);

	// LoadLibrary has the same relative address in all processes, hence we can use our "own" address.

	Pointer fnPtr(reinterpret_cast<void*>(&LoadLibraryW));

	DWORD result = SpawnThread(fnPtr, namePtr, false);

	FreeMemory(namePtr);

	return result;
}

void Process::ReplaceImportAddress(std::string_view moduleName, std::string_view functionName, Pointer to)
{
	Pointer address = FindImportAddress(moduleName, functionName);

	DWORD oldAccess = _targetProcess.VirtualProtectEx(address, Pointer::Size, PAGE_EXECUTE_READWRITE);
	_ASSERT_EXPR(oldAccess == PAGE_READONLY, L"Who leaves IAT other than read only!?");

	WriteBytes(address, to);

	[[maybe_unused]] DWORD gainedAccess = _targetProcess.VirtualProtectEx(address, Pointer::Size, oldAccess);
	_ASSERT_EXPR(gainedAccess == PAGE_EXECUTE_READWRITE, L"We did not previously gain enough access!?");
}

void Process::FreeMemory(Pointer pointer)
{
	const auto equalPointer = [&](const VirtualMemory& x)->bool
	{
		return x.Address() == pointer;
	};

	const auto it = std::find_if(_regions.cbegin(), _regions.cend(), equalPointer);

	if (it == _regions.cend())
	{
		throw OutOfRangeException("No such memory allocated");
	}

	_regions.erase(it);
}

#ifdef _WIN64
Pointer Process::InjectX64(Pointer origin, size_t nops, std::span<uint8_t> code)
{
	const size_t codeSize = code.size_bytes();
	const size_t bytesRequired = codeSize + JumpOpSize;

	_ASSERT(nops < codeSize);

	Pointer target = AllocateMemory(bytesRequired);

	{
		ByteStream codeWithJumpBack(code);

		codeWithJumpBack << JumpAbsolute(origin + JumpOpSize);

		WriteBytes(target, codeWithJumpBack);

		_targetProcess.FlushInstructionCache(target, bytesRequired);
	}

	{
		ByteStream detour(JumpAbsolute(target));
		detour.Add(nops, X86::Nop);

		WriteBytes(origin, detour);

		_targetProcess.FlushInstructionCache(origin, detour.Size());
	}

	Log << Logger::Color::Cyan << "Injected" << JumpOpSize + nops << "bytes to" << origin;
	return target;
}

Pointer Process::InjectX64(size_t offset, size_t nops, std::span<uint8_t> code)
{
	return InjectX64(Address(offset), nops, code);
}

Pointer Process::InjectX64(std::wstring_view module, size_t offset, size_t nops, std::span<uint8_t> code)
{
	return InjectX64(Address(module, offset), nops, code);
}
#else
Pointer Process::InjectX86(Pointer origin, size_t nops, std::span<uint8_t> code)
{
	const size_t codeSize = code.size_bytes();
	const size_t bytesRequired = codeSize + JumpOpSize;

	_ASSERT(nops < codeSize);

	Pointer target = AllocateMemory(bytesRequired);

	{
		ByteStream codeWithJumpBack(code);

		codeWithJumpBack << JumpOp(target + codeSize, origin + JumpOpSize);

		WriteBytes(target, codeWithJumpBack);

		_targetProcess.FlushInstructionCache(target, codeWithJumpBack.Size());
	}

	{
		ByteStream detour(JumpOp(origin, target));
		detour.Add(nops, X86::Nop);

		WriteBytes(origin, detour);

		_targetProcess.FlushInstructionCache(origin, detour.Size());
	}

	Log << Logger::Color::Cyan << "Injected" << JumpOpSize + nops << "bytes to" << origin;
	return target;
}


Pointer Process::InjectX86(size_t from, size_t nops, std::span<uint8_t> code)
{
	return InjectX86(Address(from), nops, code);
}

Pointer Process::InjectX86(std::wstring_view module, size_t offset, size_t nops, std::span<uint8_t> code)
{
	return InjectX86(Address(module, offset), nops, code);
}
#endif


DWORD Process::WairForExit(std::chrono::milliseconds timeout)
{
	Win32Event event;

	HANDLE handles[2] = { _targetProcess.Value(), event.Value() };

	DWORD result = WaitForMultipleObjects(2, handles, false, static_cast<DWORD>(timeout.count()));

	switch (result)
	{
		case WAIT_OBJECT_0:
		{
			DWORD exitCode = _targetProcess.ExitCode();

			if (exitCode != 0)
			{
				std::string message = WindowsErrorCategory().message(exitCode);
				Log << Logger::Color::Red 
					<< "Process" << _pid << "exited with code" << exitCode << "message:" << message;
			}
			else
			{
				Log << Logger::Color::LightMagenta << "Process" << _pid << "exited with code 0";
			}

			return exitCode;
		}
		case WAIT_OBJECT_0 + 1u:
		{
			throw Win32Exception("Aborted", ERROR_CANCELLED);
		}
		case WAIT_TIMEOUT:
		{
			Log << "Waiting for" << _pid << "timed out" ;
			return WAIT_TIMEOUT;
		}
		case WAIT_FAILED:
		{
			throw Win32Exception("WaitForMultipleObjects");
		}
	}

	throw Win32Exception("WaitForMultipleObjects", result);
}