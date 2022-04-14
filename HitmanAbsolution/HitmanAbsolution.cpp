#include "../Mega.pch"

/*
	Infinite ammo in Hitman Absolution
	Tested with Steam version SHA-256
	58607bbc54bd577c752ee0f04a8cfec6873855639c43d4a24bd2d1b6c693e9da
*/

int wmain(int argc, wchar_t** argv)
{
	try
	{
		const CmdArgs args(argc, argv,
		{
			{ L"infstealth", L"Infinite stealth" },
			{ L"infhealth", L"Infinite health" },
			{ L"infammo", L"Infinite ammo" },
		});

		Process process(L"HMA.exe");

		if (args.Contains(L"infstealth"))
		{
			process.Fill(0x60A926, 0x60A929, X86::Nop);
		}

		if (args.Contains(L"infhealth"))
		{
			process.Fill(0x4082E1, 0x4082E3, X86::Nop);
		}

		if (args.Contains(L"infammo"))
		{
			process.ChangeByte(0x5ECEAD, X86::DecEax, X86::IncEax);
		}
	}
	catch (const CmdArgs::MissingArguments& e)
	{
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
