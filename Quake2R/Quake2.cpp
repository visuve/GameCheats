#include "HackLib.hpp"

/*
	NOTES: It is quite weird that the hacks disappear when a save is loaded.
	It could be, that the whole module is reloaded on loading a save.
	I should hack a more persistent way.
*/

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CommandLine args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "999 ammo always" },
		{ "infarmor", typeid(std::nullopt), "999 armor always" }
	});

	DWORD pid = System::WaitForExe(L"quake2ex_steam.exe");

	Process process(pid);

	if (!process.Verify("8e59b68b4d26fd0d2db1b23ac2441c7cfb810045d96afc530c46a2bf4194fc2e"))
	{
		LogError << "Expected Quake 1 Remake (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infammo"))
	{
		process.ChangeBytes(
			L"game_x64.dll",
			0xF2199,
			ByteStream("43 8B 94 81 80 0A 00 00 8B C2 41 2B C2 43 89 84 81 80 0A 00 00"),
			ByteStream("BA E7 03 00 00 90 90 90 8B C2 90 90 90 43 89 84 81 80 0A 00 00"));
	}

	if (args.Contains("infarmor"))
	{
		process.ChangeBytes(
			L"game_x64.dll",
			0x5E156,
			ByteStream("74 34 41 2B C4 48 83 7C 24 58 00"),
			ByteStream("C7 01 E7 03 00 00 EB 2E 90 90 90"));
	}

	return 0;
}
