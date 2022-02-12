#include "MaxPayneTrainer-PCH.hpp"
#include "../HackLib/Process.hpp"
#include "../HackLib/OpCodes.hpp"

/*
	Infinite ammo in Max Payne
	Tested wit version v1.05 (Steam) with SHA-256
	e0b3b859c28adbf510dfc6285e1667173aaa7b05ac66a62403eb96d50eefae7b
*/

int wmain()
{
	try
	{
		Process process(L"maxpayne.exe");
		BYTE* gunReloadOp = process.BaseAddress() + 0x34829D;

		if (process.Read<BYTE>(gunReloadOp) != X86::SubGvEv) // sub edx, eax
		{
			throw LogicException("Gun reloading OP-code is not SUB");
		}

		process.Write<BYTE>(gunReloadOp, X86::AddGvEv); // add edx, eax
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
