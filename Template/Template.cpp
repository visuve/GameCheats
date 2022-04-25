#include "HackLib.hpp"

int wmain(int argc, wchar_t** argv)
{
	try
	{
		// Use this as a template, or start hacking away!

		const CmdArgs args(argc, argv,
		{
			{ L"calculator", typeid(std::nullopt), L"Just an example" },
		//	{ L"apihook", typeid(std::nullopt), L"Just an example" }
		});

		if (args.Contains(L"calculator"))
		{

			DWORD pid = System::Instance().WaitForWindow(L"Calculator");

			Process process(pid);

			Pointer ptr = process.AllocateMemory(4096);


		//	process.Verify("E7760F103569E1D70D011C8137CD8BCAB586980615AB013479F72C3F67E28534");
		}

		//if (args.Contains(L"apihook"))
		//{
		//	DWORD pid = System::Instance().WaitForExe(L"MinimalWin32Example.exe");

		//	Process process(pid);

		//	auto iid = process.FindImport("kernel32.dll");


		//	Pointer function = process.FindFunction(iid, "Beep");
		//	std::cout << function << std::endl;

		//	Pointer callingPoint = process.Address(0x101A);

		//	std::cout << callingPoint - function << std::endl;
		//	std::cout << function - callingPoint << std::endl;

		//	
		//	Pointer paska = function;
		//	Pointer paska2 = callingPoint - function;
		//	Pointer paska3 = function - process.Address(0);

		//	for (uint8_t& x : paska)
		//	{
		//		x &= 255;
		//	}
		//	for (uint8_t& x : paska2)
		//	{
		//		x &= 255;
		//	}

		//	

		//	std::cout << paska << std::endl;
		//	std::cout << paska2 << std::endl;
		//	std::cout << paska3 << std::endl;

		//	std::cout << "HAH!" << std::endl;

		//}
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
