#include "HackLib.hpp"

int wmain(int argc, wchar_t** argv)
{
	try
	{
		const CmdArgs args(argc, argv,
		{
			{ L"patch", typeid(std::filesystem::path), L"Remove XR_3DA.exe administrator requirement" },
		});

		if (args.Contains(L"patch"))
		{
			std::filesystem::path path = args.Get<std::filesystem::path>(L"patch");

			if (SHA256(path) != "b789f5b63cbf403cd986710e94838e5cb0b631ba31696c382a4575dee242971f")
			{
				std::cout << "Cheksum mismatch. Won't patch, will definetely break." << std::endl;
			}

			FsOps::Stab(path, 0x17D73F, "asInvoker\"       ");

			if (SHA256(path) != "52d325e3fbf0f468062090b9d594457e3fe0eb80827d49a157e745fa7f3da3ea")
			{
				std::cout << "Cheksum mismatch. Stabbing the .exe failed :-( The game might be broken." << std::endl;
			}
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
