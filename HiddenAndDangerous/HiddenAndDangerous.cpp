#include "HiddenAndDangerous-PCH.hpp"

/*
	Add ammo in Hidden & Dangerous Deluxe
	Tested with version v1.51 Deluxe (Steam), SHA-256
	4fd0b4e26fd23fcb827a5ab96a4f49d84e2ea07eaf1e3baf10285e3648c63825
*/

int wmain(int argc, wchar_t** argv)
{
	if (argc <= 1)
	{
		return ERROR_BAD_ARGUMENTS;
	}

	try
	{
		CmdArgs args(argc, argv);

		Process process(L"hde.exe");

		if (args.Contains(L"totalammo"))
		{
			for (uint32_t x : { 0x164, 0x160, 0x15C, 0x158 })
			{
				Pointer totalAmmo = process.ResolvePointer(0x000FA57Cu, 0x220u, 0xCu, 0x68u, x, 0x5Cu, 0x4u, 0x18u);
				process.Write(totalAmmo, 999u);
			}
		}
		else if (args.Contains(L"magammo"))
		{
			for (uint32_t x : { 0x164, 0x160, 0x15C, 0x158 })
			{
				Pointer magAmmo = process.ResolvePointer(0x000FA57Cu, 0x220u, 0xCu, 0x68u, x, 0x5Cu, 0x4u, 0x1Cu);
				process.Write(magAmmo, 999u);
			}
		}
		else if (args.Contains(L"infammo"))
		{
			// Fill total ammo reducing function with NOPs
			process.Fill(0x00059D90, 0x00059DB4, X86::Nop);

			// Skip clip ammo reducing function by jumping directly to its return
			// The function cannot be overwritten with nops, because during loads, the mags will reset to zero
			Pointer ptr = process.Address(0x0000A614u);
			uint8_t code[] = { 0xE8, 0xCB, 0xF7, 0x04, 0x00 };
			process.Write(ptr, code);
		}
		else
		{
			return ERROR_BAD_ARGUMENTS;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
