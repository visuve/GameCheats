#include "Win32Process.hpp"
#include "Win32Process.hpp"
#include "Win32Process.hpp"
#include "System.hpp"
#include "Win32Process.hpp"

Win32Process::Win32Process(DWORD desiredAccess, DWORD pid) :
	Win32Handle(OpenProcess(desiredAccess, false, pid))
{
}

HANDLE Win32Process::Value() const
{
	return _handle;
}

std::filesystem::path Win32Process::Path() const
{
	// https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
	DWORD size = 0x7FFF;
	std::wstring buffer(size, 0); // A gigantic buffer, but I do not care for now

	if (!QueryFullProcessImageNameW(_handle, 0, buffer.data(), &size))
	{
		throw Win32Exception("QueryFullProcessImageNameW");
	}

	_ASSERT(buffer.size() >= size);

	buffer.resize(size);

	return buffer;
}

DWORD Win32Process::WaitForInputIdle(std::chrono::milliseconds timeout) const
{
	return ::WaitForInputIdle(_handle, static_cast<DWORD>(timeout.count()));
}

DWORD Win32Process::WaitForExit(std::chrono::milliseconds timeout) const
{
	return ::WaitForSingleObject(_handle, static_cast<DWORD>(timeout.count()));
}

size_t Win32Process::ReadProcessMemory(Pointer pointer, void* buffer, size_t size) const
{
	SIZE_T bytesRead = 0;

	if (!::ReadProcessMemory(_handle, pointer, buffer, size, &bytesRead))
	{
		throw Win32Exception("ReadProcessMemory");
	}

	return bytesRead;
}

size_t Win32Process::WriteProcessMemory(Pointer pointer, const void* value, size_t size) const
{
	SIZE_T bytesWritten = 0;

	if (!::WriteProcessMemory(_handle, pointer, value, size, &bytesWritten))
	{
		throw Win32Exception("WriteProcessMemory");
	}

	return bytesWritten;
}

MEMORY_BASIC_INFORMATION Win32Process::VirtualQueryEx(Pointer pointer) const
{
	MEMORY_BASIC_INFORMATION mbi;
	Clear(mbi);

	constexpr SIZE_T size = sizeof(MEMORY_BASIC_INFORMATION);

	if (::VirtualQueryEx(_handle, pointer, &mbi, size) != size)
	{
		throw Win32Exception("VirtualQueryEx");
	}

	return mbi;
}

DWORD Win32Process::VirtualProtectEx(Pointer pointer, size_t size, DWORD newAccess) const
{
	DWORD oldAccess = 0;

	if (!::VirtualProtectEx(_handle, pointer, size, newAccess, &oldAccess))
	{
		throw Win32Exception("VirtualProtectEx");
	}

	return oldAccess;
}

HANDLE Win32Process::CreateRemoteThread(Pointer startAddress, Pointer parameter) const
{
	auto sa = reinterpret_cast<LPTHREAD_START_ROUTINE>(startAddress.Value());

	HANDLE thread = ::CreateRemoteThread(_handle, nullptr, 0, sa, parameter, 0, nullptr);

	if (!thread)
	{
		throw Win32Exception("CreateRemoteThread");
	}

	return thread;
}

void Win32Process::FlushInstructionCache(Pointer address, size_t size) const
{
	if (!::FlushInstructionCache(_handle, address, size))
	{
		throw Win32Exception("FlushInstructionCache");
	}
}

DWORD Win32Process::ExitCode()
{
	DWORD exitCode = 0;

	bool result = GetExitCodeProcess(_handle, &exitCode);

	Reset(); // There is nothing to do beyond this point

	if (!result)
	{
		DWORD status = GetLastError();

		if (status != STILL_ACTIVE)
		{
			throw Win32Exception("GetExitCodeProcess", status);
		}
	}

	return exitCode;
}