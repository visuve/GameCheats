#include "Exceptions.hpp"
#include "Win32Thread.hpp"

Win32Thread::Win32Thread(HANDLE handle) :
	Win32Handle(handle)
{
}

Win32Thread::Win32Thread(DWORD identifier, DWORD access) :
	Win32Handle(OpenThread(access, true, identifier)),
	_identifier(identifier)
{
}

Win32Thread::Win32Thread(std::function<uint32_t(std::stop_token)> callback) :
	Win32Handle(CreateThread(nullptr, 0, ThreadProcedure, this, 0, &_identifier)),
	_callback(std::bind(callback, _stopSource.get_token()))
{
}

Win32Thread::~Win32Thread()
{
	if (_callback) // Yuck...
	{
		_stopSource.request_stop();

		WaitForSingleObject(_handle, INFINITE);
	}
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

	if (!result)
	{
		DWORD status = GetLastError();

		if (status != STILL_ACTIVE)
		{
			throw Win32Exception("GetExitCodeThread", status);
		}
	}

	return exitCode;
}

DWORD Win32Thread::Identifier()
{
	if (!_identifier)
	{
		_identifier = GetThreadId(_handle);

		if (!_identifier)
		{
			throw Win32Exception("GetThreadId");
		}
	}

	return _identifier;
}

DWORD Win32Thread::ThreadProcedure(void* context)
{
	auto self = reinterpret_cast<Win32Thread*>(context);

	if (!self)
	{
		throw RuntimeException("Thread context was null!");
	}

	if (!self->_callback)
	{
		throw RuntimeException("Thread callback was null!");
	}

	return self->_callback();
}