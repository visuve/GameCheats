#include "MaxPayne-PCH.hpp"

/*
	Infinite ammo in Max Payne
	Tested with version v1.05 (Steam) with SHA-256
	e0b3b859c28adbf510dfc6285e1667173aaa7b05ac66a62403eb96d50eefae7b
*/

int wmain()
{
	try
	{
		Process process(L"maxpayne.exe");
		process.ChangeByte<0x34829D>(X86::SubGvEv, X86::AddGvEv);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
