#include "HackLib.hpp"

int wmain(int argc, wchar_t** argv)
{
	try
	{
		const CmdArgs args(argc, argv,
		{
			{ L"reloadadds", typeid(std::nullopt), L"Reloading adds to total ammo" },
			{ L"infammo", typeid(std::nullopt), L"Ammunition is never reduced" },
			{ L"infbullettime", typeid(std::nullopt), L"Infinite \"bullet time\"" },
		});

		Process process(L"maxpayne.exe");

		if (!process.Verify("e0b3b859c28adbf510dfc6285e1667173aaa7b05ac66a62403eb96d50eefae7b"))
		{
			std::cerr << "Expected Max Payne v1.05 (Steam)" << std::endl;
			return ERROR_REVISION_MISMATCH;
		}

		if (args.Contains(L"reloadadds") || args.Contains(L"infammo"))
		{
			// Reload adds ammo instead of consumes
			// Also makes painkillers & throwables unlimited
			process.ChangeByte(0x34829D, X86::SubGvEv, X86::AddGvEv);
		}
		
		if (args.Contains(L"infammo"))
		{
			// The ammo never decreases
			process.Fill(0x357F50, 0x357F5C, X86::Nop);
		}

		if (args.Contains(L"infbullettime"))
		{
			process.Fill(0x4CED0, 0x4CEDC, X86::Nop);
		}
	}
	catch (const CmdArgs::Exception& e)
	{
		std::cerr << '\n' << e.what() << "!\n" << std::endl;
		std::wcerr << e.Usage() << std::endl;
		return ERROR_BAD_ARGUMENTS;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
