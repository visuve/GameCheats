#include "ByteStream.hpp"
#include "OpCodes.hpp"
#include "Process.hpp"
#include "SHA256.hpp"
#include "System.hpp"

#define UNUSED(x) (void)x;

Process::Process(DWORD pid) :
	_pid(pid),
	_module(System::ModuleEntryByPid(pid)),
	_handle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid))
{
	if (!_handle)
	{
		throw Win32Exception("OpenProcess");
	}
}

Process::Process(std::wstring_view name) :
	Process(System::PidByName(name))
{
}

Process::~Process()
{
	if (_handle)
	{
		_threads.clear();

		for (Pointer memory : _memory)
		{
			bool result = VirtualFreeEx(_handle.Get(), memory, 0, MEM_RELEASE);
			_ASSERT(result);
			UNUSED(result);
		}
	}
}

std::filesystem::path Process::Path() const
{
	// https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
	DWORD size = 0x7FFF;
	std::wstring buffer(size, 0); // A gigantic buffer, but I do not care for now

	if (!QueryFullProcessImageNameW(_handle.Get(), 0, buffer.data(), &size))
	{
		throw Win32Exception("GetModuleFileNameEx");
	}

	_ASSERT(buffer.size() >= size);

	buffer.resize(size);

	return buffer;
}

void Process::WaitForIdle()
{
	DWORD result = 0;

	do
	{
		result = WaitForInputIdle(_handle.Get(), 1000);

	} while (result == WAIT_TIMEOUT);

	if (result != 0)
	{
		throw Win32Exception("WaitForInputIdle", result);
	}

	for (DWORD i = 0x40; i < 0x1000; i *= 2)
	{
		Beep(i, 75u);
	}
}

bool Process::Verify(std::string_view expectedSHA256) const
{
	return SHA256(Path()) == expectedSHA256;
}

MODULEENTRY32W Process::FindModule(std::wstring_view name) const
{
	return System::ModuleEntryByName(_pid, name);
}

IMAGE_NT_HEADERS Process::NtHeader() const
{
	Pointer module(_module.modBaseAddr);

	const auto dosHeader = Read<IMAGE_DOS_HEADER>(module);

	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
	{
		throw LogicException("Invalid DOS header");
	}

	const auto ntHeaders = Read<IMAGE_NT_HEADERS>(module + dosHeader.e_lfanew);

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

IMAGE_IMPORT_DESCRIPTOR Process::FindImport(std::string_view moduleName) const
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

		if (_strnicmp(moduleName.data(), buffer.data(), buffer.size()) == 0)
		{
			return iid;
		}

		importDescriptorPtr += sizeof(IMAGE_IMPORT_DESCRIPTOR);

	} while (iid.Characteristics);

	throw RangeException("Import not found.");
}

Pointer Process::FindFunction(IMAGE_IMPORT_DESCRIPTOR iid, std::string_view functionName) const
{
	Pointer thunkPtr = Address(iid.OriginalFirstThunk);
	std::string buffer(MAX_PATH, '\0');

	const size_t offset = size_t(iid.OriginalFirstThunk) - iid.FirstThunk;
	IMAGE_THUNK_DATA thunk = {};

	do
	{
		thunk = Read<IMAGE_THUNK_DATA>(thunkPtr);

		if (!thunk.u1.Function)
		{
			break;
		}

		Read(Address(thunk.u1.AddressOfData + 2), buffer.data(), buffer.size());

		if (_strnicmp(functionName.data(), buffer.data(), buffer.size()) == 0)
		{
			return thunkPtr - offset;
		}

		thunkPtr += sizeof(IMAGE_THUNK_DATA);

	} while (thunk.u1.Function);


	throw RangeException("Function not found.");
}

Pointer Process::FindFunction(std::string_view moduleName, std::string_view functionName) const
{
	return FindFunction(FindImport(moduleName), functionName);
}

Pointer Process::AllocateMemory(size_t size)
{
	void* address = VirtualAllocEx(_handle.Get(), nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (!address)
	{
		throw Win32Exception("VirtualAllocEx");
	}

	auto result = _memory.emplace(address);

	_ASSERT_EXPR(result.second, L"Catastrophic failure, pointer already existed!");

	MEMORY_BASIC_INFORMATION info = {};

	if (VirtualQueryEx(_handle.Get(), address, &info, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(MEMORY_BASIC_INFORMATION))
	{
		std::cout << "Allocated " << info.RegionSize << " bytes at 0x" << address << std::endl;
	}

	return *result.first;
}

DWORD Process::CreateThread(Pointer address, Pointer parameter, bool detached)
{
	auto startAddress = reinterpret_cast<LPTHREAD_START_ROUTINE>(address.Value());
	
	Handle thread(CreateRemoteThread(_handle.Get(), nullptr, 0, startAddress, parameter, 0, 0));

	if (!thread)
	{
		throw Win32Exception("CreateRemoteThread");
	}

	if (detached)
	{
		auto result = _threads.emplace(std::move(thread));
		_ASSERT_EXPR(result.second, L"Catastrophic failure, thread already existed!");
		return 0;
	}

	if (!WaitForSingleObject(thread.Get(), INFINITE))
	{
		throw Win32Exception("WaitForSingleObject");
	}

	DWORD exitCode = 0;

	if (!GetExitCodeThread(thread.Get(), &exitCode))
	{
		throw Win32Exception("GetExitCodeThread");
	}

	return exitCode;
}

DWORD Process::InjectLibrary(std::string_view name)
{
	Pointer namePtr = AllocateMemory(name.size() + 1); // +1 to include null terminator
	Write(namePtr, name.data(), name.size());

	// LoadLibrary has the same relative address in all processes, hence we can use our "own" address.
	// The FindFunction does not appear to yield same results.
	Pointer fnPtr(&LoadLibraryA);

	DWORD result = CreateThread(fnPtr, namePtr);

	FreeMemory(namePtr);

	return result;
}

void Process::FreeMemory(Pointer pointer)
{
	const auto it = std::find(_memory.cbegin(), _memory.cend(), pointer);

	if (it == _memory.cend())
	{
		throw OutOfRangeException("No such memory allocated!");
	}

	_memory.erase(it);

	if (!VirtualFreeEx(_handle.Get(), pointer, 0, MEM_RELEASE))
	{
		throw Win32Exception("VirtualFreeEx");
	}
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

		if (!FlushInstructionCache(_handle.Get(), target, bytesRequired))
		{
			throw Win32Exception("FlushInstructionCache");
		}
	}

	{
		ByteStream detour(JumpAbsolute(target));
		detour.Add(nops, X86::Nop);

		WriteBytes(origin, detour);

		if (!FlushInstructionCache(_handle.Get(), origin, detour.Size()))
		{
			throw Win32Exception("FlushInstructionCache");
		}
	}

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
Pointer Process::InjectX86(Pointer from, size_t nops, std::span<uint8_t> code)
{
	const size_t codeSize = code.size_bytes();
	const size_t bytesRequired = codeSize + JumpOpSize;

	_ASSERT(nops < codeSize);

	Pointer target = AllocateMemory(bytesRequired);

	{
		ByteStream codeWithJumpBack(code);

		codeWithJumpBack << JumpOp(target + codeSize, from + JumpOpSize);

		WriteBytes(target, codeWithJumpBack);

		if (!FlushInstructionCache(_handle.Get(), target, codeWithJumpBack.Size()))
		{
			throw Win32Exception("FlushInstructionCache");
		}
	}

	{
		ByteStream detour(JumpOp(from, target));
		detour.Add(nops, X86::Nop);

		WriteBytes(from, detour);

		if (!FlushInstructionCache(_handle.Get(), from, detour.Size()))
		{
			throw Win32Exception("FlushInstructionCache");
		}
	}

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
	DWORD waitResult = WaitForSingleObject(_handle.Get(), static_cast<DWORD>(timeout.count()));

	switch (waitResult)
	{
		case WAIT_OBJECT_0:
		{
			DWORD exitCode = 0;

			if (GetExitCodeProcess(_handle.Get(), &exitCode))
			{
				std::cout << "Process " << _pid << " exited with code: " << exitCode << std::endl;
			}

			CloseHandle(_handle.Get());
			_handle = nullptr;

			return exitCode;
		}
		case WAIT_TIMEOUT:
		{
			std::cout << "Waiting for " << _pid << " timed out" << std::endl;
			return WAIT_TIMEOUT;
		}
	}

	throw Win32Exception("WaitForSingleObject", waitResult);
}