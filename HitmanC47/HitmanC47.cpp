#include "../Mega.pch"

/*
	NOTES: The game has it's inbuilt cheats when setting EnableCheats 1 in hitman.ini.
	However, the god mode does not prevent armor from wearing and inf ammo does not affect
	the ammo in clips.
*/
int wmain(int argc, wchar_t** argv)
{
	try
	{
		for (int i = 0; i < 10; ++i)
		{
			putc('.', stdout);
			Sleep(1000);
		}
		putc('\n', stdout);

		const CmdArgs args(argc, argv,
		{
			{ L"freeitems", L"Free items in mission menu" },
			{ L"infammo", L"Never decreasing ammunition" }
		});

		Process process(L"Hitman.Exe");

		if (!process.Verify("137c3cbf328225085cf3532819b4574714fba263a6121328de6583ba8a0648f5"))
		{
			std::cerr << "Expected Hitman: Codename 47 (Steam)" << std::endl;
			return ERROR_REVISION_MISMATCH;
		}

		if (args.Contains(L"infammo"))
		{
			// Pistols
			process.ChangeByte(process.Address(L"HitmanDlc.dlc", 0xEF52E), X86::DecEax, X86::Nop);
			
			 // SMGs & Rifles
			process.ChangeByte(process.Address(L"HitmanDlc.dlc", 0xEF729), X86::DecEcx, X86::Nop);

			// Shotguns & Snipers
			uint8_t nops[] = { X86::Nop, X86::Nop, X86::Nop, X86::Nop, X86::Nop, X86::Nop };
			process.WriteBytes(process.Address(L"HitmanDlc.dlc", 0xEF4D2), nops);
		}

		if (args.Contains(L"infarmor"))
		{
			uint8_t nops[] = { X86::Nop, X86::Nop, X86::Nop, X86::Nop, X86::Nop, X86::Nop };
			process.WriteBytes(process.Address(L"HitmanDlc.dlc", 0x11F846), nops);
		}

		if (args.Contains(L"infhealth"))
		{
			uint8_t nops[] = { X86::Nop, X86::Nop, X86::Nop, X86::Nop, X86::Nop, X86::Nop };
			process.WriteBytes(process.Address(L"HitmanDlc.dlc", 0x11F889), nops);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
