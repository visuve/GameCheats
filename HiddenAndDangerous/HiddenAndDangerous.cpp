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
		std::wstring argument(argv[1]);

		Process process(L"hde.exe");

		if (argument == L"totalammo")
		{
			for (uint32_t x : { 0x164, 0x160, 0x15C, 0x158 })
			{
				uint8_t* totalAmmo = process.ResolvePointer(0x000FA57Cu, 0x220u, 0xCu, 0x68u, x, 0x5Cu, 0x4u, 0x18u);
				process.Write(totalAmmo, 999u);
			}
		}
		else if (argument == L"magammo")
		{
			for (uint32_t x : { 0x164, 0x160, 0x15C, 0x158 })
			{
				uint8_t* magAmmo = process.ResolvePointer(0x000FA57Cu, 0x220u, 0xCu, 0x68u, x, 0x5Cu, 0x4u, 0x1Cu);
				process.Write(magAmmo, 999u);
			}
		}
		else if (argument == L"infammo")
		{
			// Fill total ammo reducing function with NOPs
			process.Fill<0x00059D90u, 0x00059DB4u, X86::OpCode>(X86::Nop);

			// Skip clip ammo reducing function by jumping directly to its return
			// The function cannot be overwritten with nops, because during loads, the mags will reset to zero
			process.Write(0x0000A614u, { 0xE8u, 0xCBu, 0xF7u, 0x04u, 0x00u });
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
