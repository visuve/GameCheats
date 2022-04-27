#include "HackLib.hpp"

int wmain(int argc, wchar_t** argv)
{
	DWORD exitCode = 0;

	try
	{
		const CmdArgs args(argc, argv,
		{
			{ L"infammo", typeid(std::nullopt), L"Ammunition is never reduced (NOTE: might be flaky)" },
			{ L"nowear", typeid(std::nullopt), L"Weapon condition is never reduced" }
		});

		DWORD pid = System::WaitForExe(L"FalloutNV.exe");

		Process process(pid);

		if (!process.Verify("3a87f92f011e5dc9179ddf733cf08be2b39ea6e5b7a8a9e3a9a72dafcc1b104d"))
		{
			LogError << "Expected Fallout New Vegas v1.4.0.525 (Steam)";
			System::BeepBurst();
			return ERROR_REVISION_MISMATCH;
		}

		process.WaitForIdle();
		System::BeepUp();

		if (args.Contains(L"infammo"))
		{
			ByteStream stream;

			// New
			// NOTE: the ammo is probably between 0 and 255, not super reliable
			stream << 0x8B << 0x4D << 0x08; // mov ecx, [ebp+08]
			stream << 0x83 << 0xF9 << 0x00; // cmp ecx, 0
			stream << 0x74 << 0x8; // je, 8 steps forward
			stream << 0x81 << 0xF9 << 0x00 << 0x01 << 0x00 << 0x00; // cmp ecx, 255
			stream << 0x7C << 0x3; // jl, 6 steps forward

			// Stolen
			stream << 0x89 << 0x48 << 0x04; // mov [eax+04], ecx

			process.Fill(0xC4F6A, 0xC4F6D, X86::Nop);
			process.InjectX86(0x2ECD4A, 1, stream);
		}

		if (args.Contains(L"nowear"))
		{
			ByteStream stream;

			// New
			stream << 0xC7 << 0x45 << 0x08 << 0xF6 << 0x3F << 0x1C << 0x46; // mov[ebp + 08], (float)9999.99

			// Stolen
			stream << 0xD9 << 0x45 << 0x08; // fld dword ptr [ebp+08]
			stream << 0xD9 << 0x58 << 0x0C; // fstp dword ptr [eax+0C]

			process.InjectX86(0x199AE, 1, stream);
		}

		exitCode = process.WairForExit();
		System::BeepDown();
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

	return exitCode;
}
