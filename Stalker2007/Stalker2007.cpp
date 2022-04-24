#include "HackLib.hpp"

int wmain(int argc, wchar_t** argv)
{
	try
	{
		const CmdArgs args(argc, argv,
		{
			{ L"patch", typeid(std::filesystem::path), L"Remove XR_3DA.exe administrator requirement" },
			{ L"infammo", typeid(std::nullopt), L"Infinite ammunition" }
			{ L"infammo", typeid(std::nullopt), L"Infinite health" }
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

			std::wcout << path << L" stabbed" << std::endl;

			return 0;
		}

		DWORD pid = System::Instance().WaitForWindow(L"S.T.A.L.K.E.R.: Shadow Of Chernobyl");

		Process process(pid);

		if (!process.Verify("52d325e3fbf0f468062090b9d594457e3fe0eb80827d49a157e745fa7f3da3ea"))
		{
			std::cerr << "Please patch the game before applying these cheats" << std::endl;
			return ERROR_REVISION_MISMATCH;
		}

		if (args.Contains(L"infammo"))
		{
			ByteStream stream;

			stream << "90 90 90 90 90 90 90"; // nop (originally add dword ptr [edi+000005C0],-38)
			stream << "FF 87 7C 05 00 00"; // inc [edi+0000057C]
			stream << "90"; // nop (not to crash)

			Pointer ptr = process.Address(L"xrGame.dll", 0x21F1CF);

			process.WriteBytes(ptr, stream);
		}

		if (args.Contains(L"infhealth"))
		{
			Pointer ptr = process.Address(L"xrGame.dll", 0x1E30C0);

			// Just overwrite call to health update function to xrGame.dll + 1E30C0
			process.WriteBytes(ptr, ByteStream("90 90 90 90 90"));
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
