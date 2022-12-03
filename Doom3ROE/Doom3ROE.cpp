#include "HackLib.hpp"

/*
	NOTE: you can allow inbuilt cheats in Doom 3 by adding
		+set com_allowconsole 1
	in the application launch options. Be wary,
	that allowing console will disable achievements in Steam!

	Also note that the cheats below are not inbuilt in Doom 3.
*/

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CmdArgs args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "Ammunition is never reduced" },
		{ "infarmor", typeid(std::nullopt), "Infinite armor" }
	});

	DWORD pid = System::WaitForWindow(L"DOOM 3");

	Process process(pid);

	if (!process.Verify("02df2b274cbf6434ef58d021689343b06f2a272182d8015580b341a1aecbeace"))
	{
		LogError << "Expected Doom 3: Resurrection of Evil (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infammo"))
	{
		Pointer shootPtr = process.Address(L"gamex86.dll", 0xBA4DD);

		// Original mov[esi + 00000BE4], eax
		process.Fill(shootPtr, X86::Nop, 6);

		Pointer sshotgunPtr = process.Address(L"gamex86.dll", 0xBB076);

		// Original sub [esi+00000BE4],eax
		process.Fill(sshotgunPtr, X86::Nop, 6);
	}

	if (args.Contains("infarmor"))
	{
		Pointer hitPtr = process.Address(L"gamex86.dll", 0x89C06);
		process.ChangeByte(hitPtr, X86::SubEvGv, X86::AddEvGv);
	}

	return 0;
}
