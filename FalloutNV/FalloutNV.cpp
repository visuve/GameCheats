#include "FalloutNV-PCH.hpp"

/*
	Infinite ammo in Fallout New Vegas v1.4.0.525
	Tested with Steam version SHA-256
	3a87f92f011e5dc9179ddf733cf08be2b39ea6e5b7a8a9e3a9a72dafcc1b104d
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

		Process process(L"FalloutNV.exe");

		if (args.Contains(L"infammo"))
		{
			Pointer ammo = process.ResolvePointer(0x00DE0774, 0x18C, 0x120, 0x4);

			ByteStream stream;

			// New
			stream << 0x3D << (ammo - 4); // cmp eax, (ammo - 4)
			stream << 0x74 << 0x6; // jmp 6 steps forward

			// Stolen
			stream << 0x8B << 0x4D << 0x08; // mov ecx, [ebp+08]
			stream << 0x89 << 0x48 << 0x04; // mov [eax+04], ecx

			process.Fill(0xC4F6A, 0xC4F6D, X86::Nop);
			process.InjectX86(0x2ECD4A, 1, stream);
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
