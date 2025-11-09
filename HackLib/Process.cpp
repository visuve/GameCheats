#include "Process.hpp"
#include "ByteStream.hpp"
#include "SHA256.hpp"
#include "Strings.hpp"
#include "Win32Thread.hpp"
#include "Win32Event.hpp"
#include "TypeHelp.hpp"

#ifdef _WIN64
extern "C" void FindFunctionAsm();
#else
extern "C" void FindFunctionAsm();
#endif

constexpr bool IsReadable(const MEMORY_BASIC_INFORMATION& mbi)
{
	if (mbi.State != MEM_COMMIT)
	{
		return false;
	}

	if (mbi.Type == MEM_PRIVATE)
	{
		return false;
	}

	return (mbi.Protect & PAGE_READONLY) ||
		(mbi.Protect & PAGE_READWRITE) ||
		(mbi.Protect & PAGE_WRITECOPY) ||
		(mbi.Protect & PAGE_EXECUTE_READ) ||
		(mbi.Protect & PAGE_EXECUTE_READWRITE) ||
		(mbi.Protect & PAGE_EXECUTE_WRITECOPY);
}

Process::Process(DWORD pid) :
	_pid(pid),
	_baseAddress(System::ModuleEntryByPid(pid).modBaseAddr),
	_targetProcess(PROCESS_ALL_ACCESS, pid)
{
	if (!_targetProcess)
	{
		throw Win32Exception("OpenProcess");
	}

	LogInfo << "Process" << pid << "opened";
}

Process::~Process()
{
	_threads.clear();
	_regions.clear();
	_targetProcess.Reset();
}

void Process::Watch(const std::set<Variable>& variables, std::chrono::milliseconds interval)
{
	auto readPtr = static_cast<void(Process::*)(size_t, void*, size_t) const>(&Process::Read);
	auto readFunction = std::bind(readPtr, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	auto watchFunction = [=](const Win32Event& event)->uint32_t
	{
		try
		{
			std::vector<Variable> copy(variables.begin(), variables.end()); // Sigh... a copy of a copy

			while (event.Wait(interval) == WAIT_TIMEOUT)
			{
				for (Variable& variable : copy)
				{
					variable.Read(readFunction);

					LogInfo << variable;
				}
			}

			return 0;
		}
		catch (const std::system_error& e)
		{
			LogError << e.what();
			return e.code().value();
		}
	};

	_threads.emplace(watchFunction);
}

void Process::Watch(const Variable& variable, std::chrono::milliseconds interval)
{
	const std::set<Variable> variables = { variable };
	Watch(std::move(variables), interval);
}

void Process::Freeze(const std::set<Variable>& variables, std::chrono::milliseconds interval)
{
	auto writePtr = static_cast<void(Process::*)(size_t, const void*, size_t) const>(&Process::Write);
	auto writeFunction = std::bind(writePtr, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	auto readPtr = static_cast<void(Process::*)(size_t, void*, size_t) const>(&Process::Read);
	auto readFunction = std::bind(readPtr, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	auto freezeFunction = [=](const Win32Event& event)->uint32_t
	{
		try
		{
			while (event.Wait(interval) == WAIT_TIMEOUT)
			{
				for (const Variable& variable : variables)
				{
					variable.Write(writeFunction);

					Sleep(1);

					Variable copy(variable);
					copy.Read(readFunction);
					LogInfo << copy;
				}
			}

			return 0;
		}
		catch (const std::system_error& e)
		{
			LogError << e.what();
			return e.code().value();
		}
	};

	_threads.emplace(freezeFunction);
}

void Process::Freeze(const Variable& variable, std::chrono::milliseconds interval)
{
	const std::set<Variable> variables = { variable };
	Freeze(std::move(variables), interval);
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

std::pair<IMAGE_DOS_HEADER, IMAGE_NT_HEADERS> Process::Headers() const
{
	const auto dosHeader = Read<IMAGE_DOS_HEADER>(_baseAddress);

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
	{
		throw LogicException("Invalid DOS header");
	}

	const auto ntHeader = Read<IMAGE_NT_HEADERS>(_baseAddress + dosHeader.e_lfanew);

	if (ntHeader.Signature != IMAGE_NT_SIGNATURE) 
	{
		throw LogicException("Invalid NT headers");
	}

#ifdef _WIN64
	if (ntHeader.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
#else
	if (ntHeader.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
#endif
	{
		throw LogicException("Invalid magic");
	}

	if (!(ntHeader.FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE))
	{	
		throw LogicException("Not an executable");
	}

	return { dosHeader, ntHeader };
}

std::vector<IMAGE_SECTION_HEADER> Process::Sections() const
{
	const auto [dosHeader, ntHeader] = Headers();

	Pointer sectionStart = _baseAddress + dosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS);

	std::vector<IMAGE_SECTION_HEADER> sections(ntHeader.FileHeader.NumberOfSections);

	if (sections.empty())
	{
		throw LogicException("No sections");
	}

	Read(sectionStart, sections.data(), sections.size() * sizeof(IMAGE_SECTION_HEADER));

	return sections;
}

IMAGE_IMPORT_DESCRIPTOR Process::FindImportDescriptor(std::string_view moduleName) const
{
	const auto [_, ntHeader] = Headers();

	IMAGE_DATA_DIRECTORY importEntry = ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	Pointer importDescriptorPtr = Address(importEntry.VirtualAddress);

	// https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-imagervatova

	IMAGE_IMPORT_DESCRIPTOR iid;
	Clear(iid);

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
		if (Strings::IEquals(moduleName, buffer.c_str()))
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
	IMAGE_THUNK_DATA thunk;
	Clear(thunk);

	do
	{
		thunk = Read<IMAGE_THUNK_DATA>(thunkPtr);

		if (!thunk.u1.Function)
		{
			break;
		}

		Read(Address(thunk.u1.AddressOfData + 2), buffer.data(), buffer.size());

		// Cast to .c_str() because the buffer contains trailing nulls
		if (Strings::IEquals(functionName, buffer.c_str()))
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

Pointer Process::FindFunctionAddress(std::string_view moduleName, std::string_view functionName)
{
	_ASSERTE(moduleName.size() <= 0x400);
	_ASSERTE(functionName.size() <= 0x400);

	std::vector<uint8_t> code = ReadFunction(FindFunctionAsm);
	Pointer codeArea = AllocateMemory(code.size());
	WriteBytes(codeArea, code);

	Pointer dataArea = AllocateMemory(0x800 + Pointer::Size);
	Write(dataArea + 0x000, moduleName.data(), moduleName.size());
	Write(dataArea + 0x400, functionName.data(), functionName.size());
	Write(dataArea + 0x800, size_t(-1));

	_targetProcess.FlushInstructionCache(dataArea, 0x800 + Pointer::Size);
	_targetProcess.FlushInstructionCache(codeArea, code.size());

	DWORD threadResult = SpawnThread(codeArea, dataArea, false);

	switch (threadResult)
	{
		case 0:
			return Read<size_t>(dataArea + 0x800);
		case 1:
			throw RuntimeException("Remote GetModuleHandleA failed");
		case 2:
			throw RuntimeException("Remote GetProcAddress failed");
	}

	throw RuntimeException("Unknown error in remote thread");
}

Pointer Process::FindBytes(std::span<const uint8_t> pattern) const
{
	Pointer address = _baseAddress;
	MEMORY_BASIC_INFORMATION mbi;
	Clear(mbi);

	const SYSTEM_INFO info = System::SystemInfo();

	const auto isValid = [&info](const Pointer& startAddress, const MEMORY_BASIC_INFORMATION& mbi)->bool
	{
		const bool validSize = mbi.RegionSize && mbi.RegionSize % info.dwPageSize == 0;
		const size_t minAddress = reinterpret_cast<size_t>(info.lpMinimumApplicationAddress);
		const size_t maxAddress = reinterpret_cast<size_t>(info.lpMaximumApplicationAddress);
		const Pointer endAddress = startAddress + mbi.RegionSize;
		
		if (validSize && startAddress > minAddress && endAddress < maxAddress)
		{
			return true;
		}

		LogDebug << "Invalid region" << startAddress << '-' << (startAddress + mbi.RegionSize);
		return false;
	};

	do
	{
		address += mbi.RegionSize; // Advance to next region
		mbi = _targetProcess.VirtualQueryEx(address);

		const Pointer regionStart(mbi.BaseAddress);

		if (!IsReadable(mbi))
		{
			LogDebug << "Unreadable region" << regionStart << '-' << (regionStart + mbi.RegionSize);
			continue;
		}

		const std::vector<uint8_t> data = ReadBytes(regionStart, mbi.RegionSize);
		const auto it = std::search(data.cbegin(), data.cend(), pattern.begin(), pattern.end());

		if (it != data.end())
		{
			size_t offset = std::distance(data.begin(), it);

			return regionStart + offset;
		}

	} while (isValid(address, mbi));

	throw RangeException("Bytes not found");
}

Pointer Process::AllocateMemory(size_t size)
{
	auto result = _regions.emplace(_targetProcess, size);

	if (!result.second)
	{
		throw RuntimeException("Catastrophic failure, pointer already existed");
	}

	MEMORY_BASIC_INFORMATION info = result.first->Query();

	LogInfo << "Allocated" << info.RegionSize << "bytes at" << result.first->Address();

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

DWORD Process::SpawnThread(Pointer startAddress, Pointer parameter, bool detached)
{
	HANDLE bare = _targetProcess.CreateRemoteThread(startAddress, parameter);

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

void Process::ReplaceImportAddress(Pointer from, Pointer to)
{
	DWORD oldAccess = _targetProcess.VirtualProtectEx(from, Pointer::Size, PAGE_EXECUTE_READWRITE);
	_ASSERT_EXPR(oldAccess == PAGE_READONLY, L"Who leaves IAT other than read only!?");

	WriteBytes(from, to);

	[[maybe_unused]] DWORD gainedAccess = _targetProcess.VirtualProtectEx(from, Pointer::Size, oldAccess);
	_ASSERT_EXPR(gainedAccess == PAGE_EXECUTE_READWRITE, L"We did not previously gain enough access!?");
}

void Process::ReplaceImportAddress(std::string_view moduleName, std::string_view functionName, Pointer to)
{
	Pointer from = FindImportAddress(moduleName, functionName);
	return ReplaceImportAddress(from, to);
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
Pointer Process::InjectX64(Pointer origin, size_t nops, std::span<const uint8_t> code)
{
	const size_t codeSize = code.size_bytes();
	const size_t bytesRequired = codeSize + JumpOpSize;

	_ASSERT(nops < codeSize);

	Pointer trampoline = AllocateMemory(bytesRequired);

	{
		ByteStream payload(code);

		payload << JumpAbsolute(origin + JumpOpSize);

		WriteBytes(trampoline, payload);

		_targetProcess.FlushInstructionCache(trampoline, payload.Size());
	}

	{
		ByteStream relay(JumpAbsolute(trampoline));
		relay.Add(nops, X86::Nop);

		WriteBytes(origin, relay);

		_targetProcess.FlushInstructionCache(origin, relay.Size());
	}

	LogInfo << "Injected" << JumpOpSize + nops << "bytes to" << origin;
	return trampoline;
}

Pointer Process::InjectX64(size_t offset, size_t nops, std::span<const uint8_t> code)
{
	return InjectX64(Address(offset), nops, code);
}

Pointer Process::InjectX64(std::wstring_view module, size_t offset, size_t nops, std::span<const uint8_t> code)
{
	return InjectX64(Address(module, offset), nops, code);
}
#else
Pointer Process::InjectX86(Pointer origin, size_t nops, std::span<const uint8_t> code)
{
	const size_t codeSize = code.size_bytes();
	const size_t bytesRequired = codeSize + JumpOpSize;

	_ASSERT(nops < codeSize);

	Pointer trampoline = AllocateMemory(bytesRequired);

	{
		ByteStream payload(code);

		payload << JumpOp(trampoline + codeSize, origin + JumpOpSize);

		WriteBytes(trampoline, payload);

		_targetProcess.FlushInstructionCache(trampoline, payload.Size());
	}

	{
		ByteStream relay(JumpOp(origin, trampoline));
		relay.Add(nops, X86::Nop);

		WriteBytes(origin, relay);

		_targetProcess.FlushInstructionCache(origin, relay.Size());
	}

	LogInfo << "Injected" << JumpOpSize + nops << "bytes to" << origin;
	return trampoline;
}


Pointer Process::InjectX86(size_t from, size_t nops, std::span<const uint8_t> code)
{
	return InjectX86(Address(from), nops, code);
}

Pointer Process::InjectX86(std::wstring_view module, size_t offset, size_t nops, std::span<const uint8_t> code)
{
	return InjectX86(Address(module, offset), nops, code);
}
#endif

std::vector<uint8_t> Process::ReadFunction(void(*function)(void), size_t size)
{
	Pointer address(reinterpret_cast<uint8_t*>(function));

	LogInfo << "Reading function @" << address;

	Win32Process process(PROCESS_VM_READ, GetCurrentProcessId());

#ifdef _DEBUG // In debug mode MSVC just generates a jump table
	uint8_t jump[5] = {};
	process.ReadProcessMemory(address, jump, sizeof(jump));

	if (jump[0] == X86::JmpJz)
	{
		// Make absolute
		address += jump[1] | (jump[2] << 8) | (jump[3] << 16) | (jump[4] << 24);
		address += 5; // Size of relative jump

		LogDebug << function << "jumps to" << address;
	}
#endif

	size_t bytesRead = 0;
	std::vector<uint8_t> result(size);

	if (size > 0)
	{
		bytesRead = process.ReadProcessMemory(address, result.data(), result.size());

		if (result.size() != bytesRead)
		{
			result.resize(bytesRead);
		}

		return result;
	}

	LogWarning << "Function size not set!";

	size_t pageSize = System::PageSize();
	std::vector<uint8_t> buffer(pageSize);

	do
	{
		bytesRead = process.ReadProcessMemory(address, buffer.data(), pageSize);

		// NOTE: this is hacky AF... The trap might not be present
		constexpr uint8_t Trap[] = { X86::Int3, X86::Int3, X86::Int3 };

		auto trap = std::search(buffer.begin(), buffer.end(), std::begin(Trap), std::end(Trap));

		if (trap == buffer.begin())
		{
			LogWarning << "Landed right in the middle of int 3s";
			break;
		}

		if (trap != buffer.end())
		{
			std::copy(buffer.begin(), trap, std::back_inserter(result));
			break;
		}

		std::copy(buffer.begin(), buffer.end(), std::back_inserter(result));
		address += bytesRead;

	} while (bytesRead);

	return result;
}

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
				LogInfo << "Process" << _pid << "exited with code" << exitCode << "message:" << message;
			}
			else
			{
				LogInfo << "Process" << _pid << "exited with code 0";
			}

			return exitCode;
		}
		case WAIT_OBJECT_0 + 1u:
		{
			throw Win32Exception("Aborted", ERROR_CANCELLED);
		}
		case WAIT_TIMEOUT:
		{
			LogInfo << "Waiting for" << _pid << "timed out" ;
			return WAIT_TIMEOUT;
		}
		case WAIT_FAILED:
		{
			throw Win32Exception("WaitForMultipleObjects");
		}
	}

	throw Win32Exception("WaitForMultipleObjects", result);
}