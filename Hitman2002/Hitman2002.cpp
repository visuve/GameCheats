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

		DWORD pid = System::Instance().WaitForWindow(L"Hitman2");

		Process process(pid);

		process.Verify("f85765f1b36734165fc22122050ca39ca1b8873f9b2d430b50a8271cde5d5136");

		if (args.Contains(L"infammo"))
		{
			ByteStream stream;

			stream << "8B 96 96 00 00 00"; // mov edx, [esi + 00000096]
			stream << "8B 8E 9E 00 00 00"; // mov ecx, [esi + 0000009E]
			stream << "8B 86 A7 00 00 00"; // mov eax, [esi + 000000A7]

			stream << "81 FC A8 F7 19 00"; // cmp esp,0019F7A8
			stream << "74 0B"; // je 11
			stream << "81 FC 6C F9 19 00"; // cmp esp,0019F96C
			stream << "74 03"; // je 3
			stream << "4A"; // dec edx
			stream << "EB 01"; // jmp 1
			stream << "42"; // inc edx
			stream << "90"; // nop

			process.InjectX86(0x76346, 14, stream);
		}

		if (args.Contains(L"infhealth"))
		{
			Pointer value = process.AllocateMemory(sizeof(double));
			process.Write(value, double(100));

			ByteStream stream;
			stream << "DD 05" << value;

			process.WriteBytes(0x1193A7, stream);
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
