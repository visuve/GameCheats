#include "CmdArgs.hpp"
#include "System.hpp"
#include "Logger.hpp"

extern int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments);

int main(int argc, char** argv)
{
	int exitCode = 0;

	try
	{
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