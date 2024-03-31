#include "HackLib.hpp"

extern "C" void InfAmmoFirearms();

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CommandLine args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "Infinite ammo" },
	});

	DWORD pid = System::WaitForWindow(L"Fallout4");

	Process process(pid);

	if (!process.Verify("55f57947db9e05575122fae1088f0b0247442f11e566b56036caa0ac93329c36"))
	{
		LogError << "Expected Fallout 4 (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infammo"))
	{
		auto infAmmo = Process::ReadFunction(InfAmmoFirearms);
		process.InjectX64(0xDFD8EC, 2, infAmmo);
	}

	process.WairForExit();

	return 0;
}
