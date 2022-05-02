#include "HackLib.hpp"

/*
	NOTES: The game has it's inbuilt cheats when setting EnableCheats 1 in hitman.ini.
	However, the god mode does not prevent armor from wearing and inf ammo does not affect
	the ammo in clips.
*/
int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	DWORD exitCode = 0;

	const CmdArgs args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "Never decreasing ammunition" },
		{ "infarmor", typeid(std::nullopt), "Never decreasing armor" },
		{ "infhealth", typeid(std::nullopt), "Never decreasing health" },
		{ "drawdistance", typeid(std::nullopt), "10x the maximum drawing distance" }
	});

	DWORD pid = System::WaitForWindow(L"Direct3D");

	Process process(pid);

	if (!process.Verify("137c3cbf328225085cf3532819b4574714fba263a6121328de6583ba8a0648f5"))
	{
		LogError << "Expected Hitman: Codename 47 (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infammo"))
	{
		// Pistols
		process.ChangeByte(process.Address(L"HitmanDlc.dlc", 0xEF52E), X86::DecEax, X86::Nop);

		// SMGs & Rifles
		process.ChangeByte(process.Address(L"HitmanDlc.dlc", 0xEF729), X86::DecEcx, X86::Nop);

		// Shotguns & Snipers
		uint8_t nops[] = { X86::Nop, X86::Nop, X86::Nop, X86::Nop, X86::Nop, X86::Nop };
		process.WriteBytes(process.Address(L"HitmanDlc.dlc", 0xEF4D2), nops);
	}

	if (args.Contains("infarmor"))
	{
		uint8_t nops[] = { X86::Nop, X86::Nop, X86::Nop, X86::Nop, X86::Nop, X86::Nop };
		process.WriteBytes(process.Address(L"HitmanDlc.dlc", 0x11F846), nops);
	}

	if (args.Contains("infhealth"))
	{
		uint8_t nops[] = { X86::Nop, X86::Nop, X86::Nop, X86::Nop, X86::Nop, X86::Nop };
		process.WriteBytes(process.Address(L"HitmanDlc.dlc", 0x11F889), nops);
	}

	if (args.Contains("drawdistance"))
	{
		Pointer value = process.AllocateMemory(sizeof(double));
		process.Write(value, double(50000.00));

		ByteStream code;

		code << "DD 05" << value;
		code << "DD 5C 24 04";
		code << "DD 81 86 01 00 00";

		process.InjectX86(L"HitmanDlc.dlc", 0x904A0, 1, code);

		exitCode = process.WairForExit();
	}

	return exitCode;
}
