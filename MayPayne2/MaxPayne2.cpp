#include "../Mega.pch"

/*
	Infinite ammo in Max Payne 2 - The Fall of Max of Payne
	Tested with Steam version SHA-256
	75b46ceaaaecd44173358654f67abd20057fd4298fc6a0d74437c0c884328fc3
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

		Process process(L"maxpayne2.exe");

		if (args.Contains(L"reloadadds") || args.Contains(L"infammo"))
		{
			Pointer reloadPtr = process.Address(L"X_GameObjectsMFC.dll", 0x7B71D);
			process.ChangeByte(reloadPtr, X86::SubGvEv, X86::AddGvEv);
		}
		
		if (args.Contains(L"infammo"))
		{
			Pointer ammoPtr = process.Address(L"X_GameObjectsMFC.dll", 0x7D265);
			process.ChangeByte(ammoPtr, X86::DecEcx, X86::Nop);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
