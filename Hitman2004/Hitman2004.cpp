#include "HackLib.hpp"

int main(int argc, char** argv)
{
	DWORD exitCode = 0;

	try
	{
		const CmdArgs args(argc, argv,
		{
			{ "infammo", typeid(std::nullopt), "Never decreasing ammunition" },
			{ "infhealth", typeid(std::nullopt), "Never decreasing health" }
		});

		DWORD pid = System::WaitForWindow(L"Hitman Contracts");

		Process process(pid);

		if (!process.Verify("189f8ce2e40603db1387266e960fcf102c479ad4de83d31b1cbfe3647b6cd702"))
		{
			LogError << "Expected Hitman Contracts";
			System::BeepBurst();
			return ERROR_REVISION_MISMATCH;
		}

		process.WaitForIdle();
		System::BeepUp();

		if (args.Contains("infammo"))
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

		if (args.Contains("dummyai"))
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

		if (args.Contains("infhealth"))
		{
			process.Fill(0x12EBBD, 0x12EBC1, X86::Nop);
		}

		exitCode = process.WairForExit();
		System::BeepDown();
	}
	catch (const CmdArgs::Exception& e)
	{
		LogError << e.what() << "\n";
		std::cerr << e.Usage();
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

	return exitCode;
}
