#include "HackLib.hpp"

extern "C" void ExampleAsmFunction();

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	int exitCode = 0;

	const CmdArgs args(givenArguments,
	{
		{ "calculator", typeid(std::nullopt), "Just an example" }
	});

	auto functionData = Process::ReadFunction(ExampleAsmFunction);
	LogInfo << ByteStream(functionData);

	if (args.Contains("calculator"))
	{
		DWORD pid = System::WaitForWindow(L"Calculator");

		Process process(pid);

		if (!process.Verify("E7760F103569E1D70D011C8137CD8BCAB586980615AB013479F72C3F67E28534"))
		{
			LogError << "You have a different calculator than was expected";
			System::BeepBurst();
			return ERROR_REVISION_MISMATCH;
		}

		process.WaitForIdle();
		System::BeepUp();
		exitCode = process.WairForExit();
	}

	return exitCode;
}