#include "Exceptions.hpp"
#include "Logger.hpp"
#include "System.hpp"
#include "Win32Event.hpp"

std::mutex Mutex;
std::list<std::string> Win32EventNames;

Win32Event::Win32Event(const std::string& name) :
	Win32Handle(CreateEventA(nullptr, true, false, name.data())),
	_name(name)
{
	std::lock_guard<std::mutex> guard(Mutex);
	Win32EventNames.emplace_back(_name);
}

Win32Event::Win32Event() :
	Win32Event(System::GenerateGuid())
{
}

Win32Event::~Win32Event()
{
	SetEvent(_handle);

	std::lock_guard<std::mutex> guard(Mutex);
	Win32EventNames.remove(_name);
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

bool Win32Event::IsSet() const
{
	return Wait(0ms) == WAIT_OBJECT_0;
}

void Win32Event::Set()
{
	[[maybe_unused]]
	bool result = SetEvent(_handle);
	_ASSERT(result);
}


