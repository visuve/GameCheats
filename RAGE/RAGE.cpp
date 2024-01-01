#include "HackLib.hpp"

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	int exitCode = 0;

	const CommandLine args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "Infinite ammo" },
		{ "crosshair", typeid(std::nullopt), "The crosshair never grows" },
		{ "freerealestate", typeid(std::nullopt), "Stuff is free" }
	});

	DWORD pid = System::WaitForExe(L"Rage64.exe");

	Process process(pid);

	if (!process.Verify("54c80b45833679c62557a3f901aa36887f703729c28ba25acd1c9a1d606d7990"))
	{
		LogError << "Expected Rage 1.0.25.4669 (Steam [64 bit])";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infammo"))
	{
		process.ChangeBytes(
			0xD81F3A,
			ByteStream("75 31"),
			ByteStream("73 31"));

		// Throwables
		process.ChangeBytes(
			0xDC4E5B,
			ByteStream("E8 20 E0 FA FF"),
			ByteStream("90 90 90 90 90"));

		// Minigun (cars)
		process.ChangeBytes(
			0xF77D65,
			ByteStream("E8 A6 B0 DF FF"),
			ByteStream("90 90 90 90 90"));

		// NOTE: the "BFG" unfortunately does not work.
		// Did not have enough interest to hack it
	}

	if (args.Contains("crosshair"))
	{
		process.ChangeBytes(
			0xED95C6,
			ByteStream("F3 41 0F 11 45 6C"),
			ByteStream("F3 41 0F 11 7D 6C"));
	}

	if (args.Contains("freerealestate"))
	{
		process.ChangeBytes(
			0xDC500C,
			ByteStream("E8 6F DE FA FF"),
			ByteStream("90 90 90 90 90"));
	}

	return exitCode;
}