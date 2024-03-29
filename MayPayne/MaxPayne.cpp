#include "HackLib.hpp"

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CommandLine args(givenArguments,
	{
		{ "reloadadds", typeid(std::nullopt), "Reloading adds to total ammo" },
		{ "infammo", typeid(std::nullopt), "Ammunition is never reduced" },
		{ "infbullettime", typeid(std::nullopt), "Infinite \"bullet time\"" },
	});

	DWORD pid = System::WaitForExe(L"maxpayne.exe");

	Process process(pid);

	if (!process.Verify("e0b3b859c28adbf510dfc6285e1667173aaa7b05ac66a62403eb96d50eefae7b"))
	{
		LogError << "Expected Max Payne v1.05 (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("reloadadds") || args.Contains("infammo"))
	{
		// Reload adds ammo instead of consumes
		// Also makes painkillers & throwables unlimited
		process.ChangeByte(0x34829D, X86::SubGvEv, X86::AddGvEv);
	}

	if (args.Contains("infammo"))
	{
		// The ammo never decreases
		process.Fill(0x357F50, 0x357F5C, X86::Nop);
	}

	if (args.Contains("infbullettime"))
	{
		process.Fill(0x4CED0, 0x4CEDC, X86::Nop);
	}

	return 0;
}
