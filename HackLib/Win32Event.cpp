#include "Exceptions.hpp"
#include "Logger.hpp"
#include "Win32Event.hpp"

std::once_flag ConsoleCtrlHandlerFlag;
std::list<std::string> EventNames;

BOOL WINAPI ConsoleHandler(_In_ DWORD signal)
{
	switch (signal)
	{
		case CTRL_C_EVENT:
			Log << "Signaled CTRL+C event";
			break;
		case CTRL_BREAK_EVENT:
			Log << "Signaled CTRL+BREAK event";
			break;
		case CTRL_CLOSE_EVENT:
			Log << "Signaled close event";
			break;
		case CTRL_LOGOFF_EVENT:
			Log << "Signaled logoff event";
			break;
		case CTRL_SHUTDOWN_EVENT:
			Log << "Signaled shutdown event";
			break;
		default:
			Log << "Signaled" << signal;
			break;
	}
	
	for (const std::string& eventName : EventNames)
	{
		HANDLE event = OpenEventA(EVENT_MODIFY_STATE, false, eventName.data());

		if (event)
		{
			bool result = SetEvent(event);
			_ASSERT_EXPR(result, "Could not set event!");
			(void)result;
		}
	}

	return true;
}
Win32Event::Win32Event(const std::string& name) :
	Win32Handle(CreateEventA(nullptr, true, false, name.data())),
	_name(name)
{
	std::call_once(ConsoleCtrlHandlerFlag, []()
	{
		SetConsoleCtrlHandler(ConsoleHandler, true);
	});

	EventNames.push_back(_name);
}

Win32Event::~Win32Event()
{
	// This is dangerous and probably not needed
	EventNames.remove(_name);
}

DWORD Win32Event::Wait(std::chrono::milliseconds timeout) const
{
	DWORD result = WaitForSingleObject(_handle, static_cast<DWORD>(timeout.count()));

	if (result == WAIT_FAILED)
	{
		throw Win32Exception("WaitForSingleObject");
	}

	return result;
}

HANDLE Win32Event::Value() const
{
	return _handle;
}