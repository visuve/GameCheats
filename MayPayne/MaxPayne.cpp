#include "MaxPayne-PCH.hpp"

/*
	Infinite ammo in Max Payne
	Tested with version v1.05 (Steam) with SHA-256
	e0b3b859c28adbf510dfc6285e1667173aaa7b05ac66a62403eb96d50eefae7b
*/

int wmain(int argc, wchar_t** argv)
{
	if (argc <= 1)
	{
		return ERROR_BAD_ARGUMENTS;
	}

	try
	{
		const CmdArgs args(argc, argv);

		Process process(L"maxpayne.exe");

		if (args.Contains(L"reloadadds") || args.Contains(L"infammo"))
		{
			// Reload adds ammo instead of consumes
			// Also makes painkillers & throwables unlimited
			process.ChangeByte(0x34829D, X86::SubGvEv, X86::AddGvEv);
		}
		
		if (args.Contains(L"infammo"))
		{
			// The ammo never decreases
			process.Fill<0x357F50, 0x357F5C>(X86::Nop);
		}

		if (args.Contains(L"infbullettime"))
		{
			process.Fill<0x4CED0, 0x4CEDC>(X86::Nop);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
