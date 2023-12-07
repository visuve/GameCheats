#include "HackLib.hpp"

extern "C" void InfAmmo();

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CmdArgs args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "Infinite ammunition" }
	});

	DWORD pid = System::WaitForWindow(L"HITMAN 3");

	Process process(pid);

	if (!process.Verify("af140fa5e5790ded4d379adc769ccbcb91d7fe82fdf4d09f30ece6edf698fea2"))
	{
		LogError << "Expected Hitman 3 (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infammo"))
	{
		auto bytes = Process::ReadFunction(InfAmmo);
		process.WriteBytes(0x3EEA6D, bytes);

	}

	return 0;
}
