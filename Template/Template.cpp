#include "HackLib.hpp"

int wmain(int argc, wchar_t** argv)
{
	try
	{
		// Use this as a template, or start hacking away!

		const CmdArgs args(argc, argv,
		{
			{ L"calculator", typeid(std::nullopt), L"Just an example" }
		});

		if (args.Contains(L"calculator"))
		{
			DWORD pid = System::WaitForWindow(L"Calculator");

			Process process(pid);

			if (!process.Verify("E7760F103569E1D70D011C8137CD8BCAB586980615AB013479F72C3F67E28534"))
			{
				LogError << "You have a different calculator than was expected";
				return ERROR_REVISION_MISMATCH;
			}

			process.WaitForIdle();

			System::BeepUp();

			DWORD result = process.WairForExit();

			System::BeepDown();

			return result;
		}
	}
	catch (const CmdArgs::Exception& e)
	{
		LogError << '\n' << e.what() << "!\n";
		std::wcerr << e.Usage();
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
		LogError << e.what();
		System::BeepBurst();
		return ERROR_PROCESS_ABORTED;
	}

	return 0;
}
