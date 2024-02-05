#include "Exceptions.hpp"
#include "Win32Thread.hpp"

Win32Thread::Win32Thread(HANDLE handle) :
	Win32Handle(handle)
{
}

void Win32Thread::Wait(std::chrono::milliseconds timeout) const
{
	DWORD result = WaitForSingleObject(_handle, static_cast<DWORD>(timeout.count()));

	if (result != WAIT_OBJECT_0)
	{
		throw Win32Exception("WaitForSingleObject", result);
	}
}

DWORD Win32Thread::ExitCode()
{
	DWORD exitCode = 0;
	bool result = GetExitCodeThread(_handle, &exitCode);

	_handle = nullptr; // There is nothing to do beyond this point

	if (!result)
	{
		DWORD status = GetLastError();

		if (status != STILL_ACTIVE)
		{
			throw Win32Exception("GetExitCodeThread", status);
		}
	}

	_handle = nullptr;
	return exitCode;
}