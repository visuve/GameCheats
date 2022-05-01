#include "Exceptions.hpp"
#include "Logger.hpp"
#include "Win32Event.hpp"

static BOOL WINAPI ConsoleHandler(_In_ DWORD);

class EventList
{
public:
	EventList()
	{
		SetConsoleCtrlHandler(ConsoleHandler, true);

#ifdef _DEBUG
		Log << "Hello, Welcome from HackLib!";
#endif
	}

	~EventList()
	{
		_ASSERT(_eventNames.empty());

#ifdef _DEBUG
		Log << "Bye!";
#endif
	}

	void Add(const std::string& name)
	{
		_eventNames.push_back(name);
	}

	void Remove(const std::string& name)
	{
		_eventNames.remove(name);
	}

	std::list<std::string> Names() const
	{
		return _eventNames;
	}

private:
	// Shouldn't need a mutex for these.
	// Also, directly adding the handles instead of names
	// resulted the assertion in SetEvent below to fail :-(
	std::list<std::string> _eventNames;
};

EventList events;

static BOOL WINAPI ConsoleHandler(_In_ DWORD signal)
{
	DWORD exitCode = ERROR_CANCELLED;

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
			// There is no sane exit code for logoff
			exitCode = static_cast<DWORD>(SCARD_E_SYSTEM_CANCELLED);
			break;
		case CTRL_SHUTDOWN_EVENT:
			Log << "Signaled shutdown event";
			exitCode = ERROR_SYSTEM_SHUTDOWN;
			break;
		default:
			Log << "Signaled" << signal;
			break;
	}

	std::list<std::string> names = events.Names();

	if (names.empty())
	{
		exit(exitCode);
	}

	for (const std::string& eventName : names)
	{
		HANDLE event = OpenEventA(EVENT_MODIFY_STATE, false, eventName.data());

		if (event)
		{
			bool result = SetEvent(event);
			_ASSERT_EXPR(result, "Could not set event");
			(void)result;
		}
	}

	return true;
}

Win32Event::Win32Event(const std::string name) :
	Win32Handle(CreateEventA(nullptr, true, false, name.data())),
	_name(name)
{
	events.Add(_name);
}

Win32Event::Win32Event() :
	Win32Event(System::GenerateGuid())
{
}

Win32Event::~Win32Event()
{
	events.Remove(_name);
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
