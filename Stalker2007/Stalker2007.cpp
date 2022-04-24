#include "HackLib.hpp"

constexpr char UnpatchedChecksum[] = "b789f5b63cbf403cd986710e94838e5cb0b631ba31696c382a4575dee242971f";
constexpr char PatchedChecksum[] = "52d325e3fbf0f468062090b9d594457e3fe0eb80827d49a157e745fa7f3da3ea";

int wmain(int argc, wchar_t** argv)
{
	try
	{
		const CmdArgs args(argc, argv,
		{
			{ L"patch", typeid(std::filesystem::path), L"Remove XR_3DA.exe administrator requirement" },
			{ L"infammo", typeid(std::nullopt), L"Infinite ammunition" },
			{ L"nowear", typeid(std::nullopt), L"Weapon condition is never reduced" },
			{ L"infhealth", typeid(std::nullopt), L"Infinite health" }
		});

		if (args.Contains(L"patch"))
		{
			std::filesystem::path path = args.Get<std::filesystem::path>(L"patch");

			if (SHA256(path) != UnpatchedChecksum)
			{
				std::cout << "Cheksum mismatch. Won't patch, will definetely break." << std::endl;
			}

			FsOps::Stab(path, 0x17D73F, "asInvoker\"       ");

			if (SHA256(path) != PatchedChecksum)
			{
				std::cout << "Cheksum mismatch. Stabbing the .exe failed :-( The game might be broken." << std::endl;
			}

			std::wcout << path << L" stabbed" << std::endl;

			return 0;
		}

		DWORD pid = System::Instance().WaitForWindow(L"S.T.A.L.K.E.R.: Shadow Of Chernobyl");

		Process process(pid);

		if (!process.Verify(PatchedChecksum))
		{
			std::cerr << "Please patch the game before applying these cheats" << std::endl;
			return ERROR_REVISION_MISMATCH;
		}
		
		size_t pageSize = System::PageSize();
		Pointer weapon = process.AllocateMemory(pageSize); // Let's just allocate a whole page

		{
			ByteStream stream;

			stream << "8B 93 38 34 00 00"; // mov edx,[ebx+00003438]
			stream << "89 15" << weapon; // mov [player], edx
			stream << "8B 93 34 34 00 00"; // mov edx,[ebx+00003434]

			// Hmmm IAT @ xrGame.dll+451278
			process.InjectX86(L"xrGame.dll", 0x3D136C, 1, stream);
		}

		if (args.Contains(L"infammo"))
		{
			ByteStream stream;

			stream << "39 3D" << weapon; // cmp dword ptr [weapon],edi
			stream << "74 10"; // je 10
			stream << "83 87 C0 05 00 00 C8";
			stream << "83 87 7C 05 00 00 FF";
			stream << "EB 06";
			stream << "FF 87 7C 05 00 00";
			stream << "90";

			process.InjectX86(L"xrGame.dll", 0x21F1CF, 9, stream);
		}

		if (args.Contains(L"nowear"))
		{
			ByteStream stream;

			stream << "39 3D" << weapon; // cmp dword ptr [weapon],edi
			stream << "75 04"; // jne 4
			stream << "DD D9"; // fstp st(0)
			stream << "D9 E8"; // fld1
			stream << "90"; // nop

			stream << "D8 87 A8 00 00 00"; // fadd dword ptr [edi+000000A8]
			stream << "D9 9F A8 00 00 00"; // fstp dword ptr [edi+000000A8]

			process.InjectX86(L"xrGame.dll", 0x21F0D9, 7, stream);
		}

		if (args.Contains(L"infhealth"))
		{
			Pointer ptr = process.Address(L"xrGame.dll", 0x1E30C0);

			// Just overwrite call to health update function to xrGame.dll + 1E30C0
			// TODO: this crashes the game upon game loads...
			process.WriteBytes(ptr, ByteStream("90 90 90 90 90"));
		}

		process.WairForExit();
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
