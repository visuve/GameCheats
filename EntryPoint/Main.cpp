#include "CmdArgs.hpp"
#include "System.hpp"
#include "Logger.hpp"
#include "Win32Event.hpp"

extern int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments);

extern std::list<std::string> Win32EventNames;

static BOOL WINAPI ConsoleHandler(DWORD signal)
{
	std::string eventType;

	switch (signal)
	{
		case CTRL_C_EVENT:
			eventType = "CTRL+C";
			break;
		case CTRL_BREAK_EVENT:
			eventType = "CTRL+BREAK";
			break;
		case CTRL_CLOSE_EVENT:
			eventType = "Window close";
			break;
		case CTRL_LOGOFF_EVENT:
			eventType = "Log off";
			break;
		case CTRL_SHUTDOWN_EVENT:
			eventType = "Shutdown";
			break;
		default:
			eventType = std::format("Signaled {0}", signal);
			break;
	}

	LogWarning << eventType << "occurred";

	for (std::string_view eventName : Win32EventNames)
	{
		HANDLE event = OpenEventA(EVENT_MODIFY_STATE, false, eventName.data());

		if (event && !SetEvent(event))
		{
			LogError << "Failed to set" << eventName;
		}
	}

	return true;
}

int main(int argc, char** argv)
{
	int exitCode = 0;

	try
	{
		if (!SetConsoleCtrlHandler(ConsoleHandler, true))
		{
			throw Win32Exception("SetConsoleCtrlHandler");
		}

		exitCode = IWillNotUseHackLibForEvil({ argv, argv + argc });
		System::BeepDown();
	}
	catch (const CmdArgs::Exception& e)
	{
		LogError << e.what() << "\n";
		std::cerr << e.Usage();
		return ERROR_BAD_ARGUMENTS;
	}
	catch (const std::system_error& e)
	{
		LogError << e.what();
		System::BeepBurst();
		return e.code().value();
	}
	catch (const std::exception& e)
	{
		LogError << e.what();
		System::BeepBurst();
		return ERROR_PROCESS_ABORTED;
	}

	return exitCode;
}