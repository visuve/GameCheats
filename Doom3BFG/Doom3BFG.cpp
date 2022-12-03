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
		{ "inflamp", typeid(std::nullopt), "Lamp battery is never reduced" },
		{ "infstamina", typeid(std::nullopt), "Lamp battery is never reduced" },
	});

	DWORD pid = System::WaitForWindow(L"DOOM 3: BFG Edition");

	Process process(pid);

	if (!process.Verify("bdca86166906063d370123e1c3b2a9a86b565dfba8bc40f05b16f23c79c29546"))
	{
		LogError << "Expected Doom 3: BFG Edition (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infammo"))
	{
		// Original: sub [esi],ecx
		process.Fill(0x32689B, 0x32689D, X86::Nop);

		// Pipe bombs
		// Original: mov[edi + esi * 8 + 000009A0], eax
		process.Fill(0x2FAC63, 0x2FAC6A, X86::Nop);
	}
	
	if (args.Contains("inflamp"))
	{
		// inc [edi+00002314]
		ByteStream bs("FF 87 14 23 00 00");

		// Original: add [edi+00002314],edx
		process.WriteBytes(0x2FBC2C, bs);
	}

	if (args.Contains("infstamina"))
	{
		// Original: fstp dword ptr [esi+00002398]
		process.Fill(0x2F184A, 0x2F1850, X86::Nop);
	}

	return 0;
}
