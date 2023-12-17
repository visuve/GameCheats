#include "HackLib.hpp"

extern "C" void InfAmmo();

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CmdArgs args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "Infinite ammunition" },
		{ "crosshair", typeid(std::nullopt), "The crosshair does not spread when shooting" },
		{ "fastfire", typeid(std::nullopt), "Semi-automatic guns can fire faster" },
		{ "lowrecoil", typeid(std::nullopt), "Low recoil after a few shots" },
		{ "godmode", typeid(std::nullopt), "Become invulnerable" }
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

	if (args.Contains("crosshair"))
	{
		process.ChangeBytes(0x3F1F1B,
			ByteStream("F3 44 0F 11 87 6C 08 00 00"),
			ByteStream("45 0F 57 C0 90 90 90 90 90"));
	}

	if (args.Contains("fastfire"))
	{
		process.ChangeBytes(0x3F217F,
			ByteStream("FF 87 50 08 00 00"),
			ByteStream("FF 8F 50 08 00 00"));
	}

	if (args.Contains("lowrecoil"))
	{
		process.ChangeBytes(0x3F2236,
			ByteStream("74 70"),  // je 70
			ByteStream("75 70")); // jne 70
	}

	if (args.Contains("godmode"))
	{
		process.ChangeBytes(0x349D17,
			ByteStream("E8 54 C1 3B 00"),
			ByteStream("B8 00 00 00 00"));
	}

	return 0;
}
