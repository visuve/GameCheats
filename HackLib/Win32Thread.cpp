#include "Win32Thread.hpp"
#include "Exceptions.hpp"

Win32Thread::Win32Thread(HANDLE handle) :
	Win32Handle(handle)
{
}

Win32Thread::Win32Thread(DWORD identifier, DWORD access) :
	Win32Handle(OpenThread(access, true, identifier)),
	_identifier(identifier)
{
}

Win32Thread::Win32Thread(std::function<uint32_t(const Win32Event&)> callback) :
	Win32Handle(CreateThread(nullptr, 0, ThreadProcedure, this, CREATE_SUSPENDED, &_identifier)),
	_callback(std::bind(callback, std::cref(_event)))
{
	// Hack: CREATE_SUSPENDED and this is to ensure that this class has been properly initialized
	ResumeThread(_handle);
}

Win32Thread::~Win32Thread()
{
	if (_callback) // Yuck...
	{
		_event.Set();

		[[maybe_unused]]
		DWORD result = WaitForSingleObject(_handle, INFINITE);

		_ASSERTE(result == WAIT_OBJECT_0);
	}
}

DWORD Win32Thread::Wait(std::chrono::milliseconds timeout) const
{
	DWORD result = WaitForSingleObject(_handle, static_cast<DWORD>(timeout.count()));

	if (result == WAIT_FAILED)
	{
		throw Win32Exception("WaitForSingleObject");
	}

	return result;
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

	_ASSERTE(self && self->_callback);

	return self->_callback();
}