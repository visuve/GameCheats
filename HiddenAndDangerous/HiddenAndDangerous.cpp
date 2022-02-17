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

		BYTE* ammoBase = process.Address(0x000FA57C);

		if (argument == L"totalammo")
		{
			for (DWORD x : { 0x164, 0x160, 0x15C, 0x158 })
			{
				BYTE* player = process.FindPointer(ammoBase, { 0x220, 0xC, 0x68, x, 0x5C, 0x4, 0x18 });
				process.Write<DWORD>(player, 999);
			}
		}
		else if (argument == L"magammo")
		{
			for (DWORD x : { 0x164, 0x160, 0x15C, 0x158 })
			{
				BYTE* player = process.FindPointer(ammoBase, { 0x220, 0xC, 0x68, x, 0x5C, 0x4, 0x1C });
				process.Write<DWORD>(player, 999);
			}
		}
		else if (argument == L"infammo")
		{
			// This tends to render reloading completely impossible
			process.Fill<X86::OpCode, 0x00059D90, 0x00059DB4>(X86::Nop);
			// process.Fill<X86::OpCode, 0x00059DC0, 0x00059DE4>(X86::Nop);
			process.Fill<X86::OpCode, 0x00059DF0, 0x00059E0A>(X86::Nop);
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
