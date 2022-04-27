#include "Win32Event.hpp"
#include "Win32Event.hpp"
#include "Exceptions.hpp"
#include "Logger.hpp"
#include "System.hpp"
#include "Win32Event.hpp"

HANDLE Handle = nullptr;

BOOL WINAPI SimpleHandler(_In_ DWORD signal)
{
	Log << "Signaled" << signal;
	
	if (Handle)
	{
		SetEvent(Handle);
	}

	return true;
}

Win32Event::Win32Event(std::wstring_view name, bool consoleEvent) :
	Win32Handle(CreateEventW(nullptr, true, false, name.data()))
{
	if (consoleEvent)
	{
		Handle = _handle;
	}

	SetConsoleCtrlHandler(SimpleHandler, true);
}

Win32Event::~Win32Event()
{
	Handle = nullptr;
	SetConsoleCtrlHandler(SimpleHandler, false);
}

DWORD Win32Event::Wait(std::chrono::milliseconds timeout) const
{
	return WaitForSingleObject(_handle, static_cast<DWORD>(timeout.count()));
}

void Win32Event::SetEvent() const
{
	if (!::SetEvent(_handle))
	{
		throw Win32Exception("SetEvent");
	}
}

HANDLE Win32Event::Value() const
{
	return _handle;
}