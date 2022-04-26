#include "HackLib.hpp"

int wmain(int argc, wchar_t** argv)
{
	try
	{
		const CmdArgs args(argc, argv,
		{
			{ L"reloadadds", typeid(std::nullopt), L"Reloading adds to total ammo" },
			{ L"infammo", typeid(std::nullopt), L"Ammunition is never reduced" }
		});

		Process process(L"maxpayne2.exe");

		if (!process.Verify("75b46ceaaaecd44173358654f67abd20057fd4298fc6a0d74437c0c884328fc3"))
		{
			LogError << "Expected Max Payne 2 - The Fall of Max of Payne (Steam)";
			return ERROR_REVISION_MISMATCH;
		}

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
	catch (const CmdArgs::Exception& e)
	{
		LogError << '\n' << e.what() << "!\n";
		std::wcerr << e.Usage();
		return ERROR_BAD_ARGUMENTS;
	}
	catch (const std::exception& e)
	{
		LogError << e.what();
		return -1;
	}

	return 0;
}
