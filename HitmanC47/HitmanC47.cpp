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
		const CmdArgs args(argc, argv,
		{
			{ L"infammo", L"Never decreasing ammunition" },
			{ L"infarmor", L"Never decreasing armor" },
			{ L"infhealth", L"Never decreasing health" },
			{ L"drawdistance", L"10x the maximum drawing distance" }
		});

		DWORD pid = Process::WaitToAppear(L"Hitman.Exe");

		Process process(pid);

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

		if (args.Contains(L"drawdistance"))
		{
			Pointer value = process.AllocateMemory(sizeof(double));
			process.Write(value, double(50000.00));

			ByteStream code;

			code << "DD 05" << value;
			code << "DD 5C 24 04";
			code << "DD 81 86 01 00 00";
			
			process.InjectX86(L"HitmanDlc.dlc", 0x904A0, 1, code);

			process.WairForExit();
		}
	}
	catch (const CmdArgs::MissingArguments& e)
	{
		std::cerr << '\n' << e.what() << "!\n" << std::endl;
		std::wcerr << e.Usage() << std::endl;
		return ERROR_BAD_ARGUMENTS;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
