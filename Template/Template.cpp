#include "../Mega.pch"

int wmain(int argc, wchar_t** argv)
{
	try
	{
		// Use this as a template, or start hacking away!

		CmdArgs args(argc, argv,
		{
			{ L"calculator", L"Just an example" }
		});

		if (args.Contains(L"calculator"))
		{

			DWORD pid = System::Instance().WaitForWindow(L"Calculator");

			Process process(pid);

			process.Verify("E7760F103569E1D70D011C8137CD8BCAB586980615AB013479F72C3F67E28534");
		}
	}
	catch (const CmdArgs::MissingArguments& e)
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
