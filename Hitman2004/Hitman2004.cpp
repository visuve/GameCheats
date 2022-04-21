#include "../Mega.pch"

int wmain(int argc, wchar_t** argv)
{
	try
	{
		const CmdArgs args(argc, argv,
		{
			{ L"infammo", L"Never decreasing ammunition" },
			{ L"infhealth", L"Never decreasing health" }
		});

		DWORD pid = System::Instance().WaitForWindow(L"Hitman Contracts");

		Process process(pid);

		if (!process.Verify("189f8ce2e40603db1387266e960fcf102c479ad4de83d31b1cbfe3647b6cd702"))
		{
			std::cerr << "Expected Hitman Contracts" << std::endl;
			return ERROR_REVISION_MISMATCH;
		}

		if (args.Contains(L"infammo"))
		{
			ByteStream code;

			// Stolen
			code << "8B 8E 83 00 00 00"; // mov ecx,[esi+00000083]
			code << "8B 86 9C 00 00 00"; // mov eax,[esi+0000009C]

			// New
			code << "81 FC A4 F4 19 00"; // cmp esp, 0019F4A4
			code << "74 0B"; // je 11
			code << "81 FC 74 F6 19 00"; // cmp esp, 0019F674
			code << "74 03"; // je 3
			code << "49"; // dec ecx
			code << "EB 01"; // jmp 1
			code << "41"; // inc ecx
			code << "90"; // nop

			process.InjectX86(0x77CD1, 8, code);
		}

		if (args.Contains(L"dummyai"))
		{
			ByteStream code;

			code << "81 FC AC F4 19 00"; // cmp esp,0019F4AC
			code << "74 01"; // je 1
			code << "C3"; // ret
			code << "56"; // push esi
			code << "8B F1"; // mov esi, ecx
			code << "8A 86 FD 00 00 00"; // mov al,[esi+000000FD]

			process.InjectX86(0x77C20, 4, code);
		}

		if (args.Contains(L"infhealth"))
		{
			process.Fill(0x12EBBD, 0x12EBC1, X86::Nop);
		}

		process.WairForExit();
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
