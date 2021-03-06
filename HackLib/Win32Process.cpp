#include "System.hpp"
#include "Win32Process.hpp"

Win32Process::Win32Process(DWORD desiredAccess, DWORD pid) :
	Win32Handle(OpenProcess(desiredAccess, false, pid))
{
}

Win32Process::~Win32Process()
{
}

std::filesystem::path Win32Process::Path() const
{
	// https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
	DWORD size = 0x7FFF;
	std::wstring buffer(size, 0); // A gigantic buffer, but I do not care for now

	if (!QueryFullProcessImageNameW(_handle, 0, buffer.data(), &size))
	{
		throw Win32Exception("GetModuleFileNameEx");
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

HANDLE Win32Process::CreateRemoteThread(Pointer address, Pointer parameter) const
{
	auto startAddress = reinterpret_cast<LPTHREAD_START_ROUTINE>(address.Value());

	HANDLE handle = ::CreateRemoteThread(_handle, nullptr, 0, startAddress, parameter, 0, 0);

	if (!handle)
	{
		throw Win32Exception("CreateRemoteThread");
	}

	return handle;
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

	_handle = nullptr; // There is nothing to do beyond this point

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