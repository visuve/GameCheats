#include "HackLib.hpp"

extern "C" void InfAmmo();

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CommandLine args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "255 ammo always" }
	});

	DWORD pid = System::WaitForExe(L"Quake_x64_steam.exe");

	Process process(pid);

	if (!process.Verify("af7557e3bba1c994f9460b820136ff5172cb153147f30a43f9f0bb8d99a5c540"))
	{
		LogError << "Expected Quake 1 Remake (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infammo"))
	{
		auto infAmmo = Process::ReadFunction(InfAmmo);

		process.InjectX64(0x1CE071, 3, infAmmo);

		process.WairForExit();
	}

	return 0;
}
