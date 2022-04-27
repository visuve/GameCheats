#include "HackLib.hpp"

int wmain(int argc, wchar_t** argv)
{
	try
	{
		const CmdArgs args(argc, argv,
		{
			{ L"infstealth", typeid(std::nullopt), L"Infinite stealth" },
			{ L"infhealth", typeid(std::nullopt), L"Infinite health" },
			{ L"infammo", typeid(std::nullopt), L"Infinite ammo" },
		});

		DWORD pid = System::WaitForExe(L"HMA.exe");

		Process process(pid);

		if (!process.Verify("58607bbc54bd577c752ee0f04a8cfec6873855639c43d4a24bd2d1b6c693e9da"))
		{
			LogError << "Expected Hitman Absolution v1.0.447 (Steam)";
			System::BeepBurst();
			return ERROR_REVISION_MISMATCH;
		}

		process.WaitForIdle();
		System::BeepUp();

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
		
		System::BeepDown();
	}
	catch (const CmdArgs::Exception& e)
	{
		LogError << '\n' << e.what() << "!\n";
		std::wcerr << e.Usage() ;
		return ERROR_BAD_ARGUMENTS;
	}
	catch (const std::system_error& e)
	{
		LogError << e.what();
		System::BeepBurst();
		return e.code().value();
	}
	catch (const std::exception& e)
	{
		LogError << e.what() ;
		System::BeepBurst();
		return ERROR_PROCESS_ABORTED;
	}

	return 0;
}
