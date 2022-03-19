#include "HM2016-PCH.hpp"

/*
	Infinite ammo in Hitman v1.15.0 (DX12)
	Tested with Steam version SHA-256
	31c33c1937b1ddc3cad21bd7d761f12f962286d55c48d3582a704ca35e2830f9
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

		Process process(L"HITMAN.exe");

		if (args.Contains(L"infammo"))
		{
			// hitman.exe+12A09C - 41 0F11 86 78030000 - movups [r14+00000378],xmm0
			// from there on up the xoring
			uint8_t bytes[] = { 0x49, 0xFF, 0xC4, 0x90 };
			process.WriteBytes(0x129FF1, bytes);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
