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
			ByteStream code;

			// Stolen
			code << 0x89 << 0x05 << 0x1D << 0x2C << 0xBC << 0x01;
			code << 0x66 << 0x89 << 0x05 << 0x1A << 0x2C << 0xBC << 0x01;
			code << 0x88 << 0x05 << 0x16 << 0x2C << 0xBC << 0x01;

			// New code
			code << 0x49 << 0xBC << 0xE7 << 0x03 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00;

			process.InjectX64(0x129FDE, 9, code);
		}

		Sleep(INFINITE);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
