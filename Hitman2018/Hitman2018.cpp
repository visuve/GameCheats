#include "HackLib.hpp"

extern "C" void InfAmmo();

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CmdArgs args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "Increasing ammunition" },
		{ "crosshair", typeid(std::nullopt), "The crosshair does not spread when shooting" },
		{ "fastfire", typeid(std::nullopt), "Semi-automatic guns can fire faster" },
		{ "lowrecoil", typeid(std::nullopt), "Low recoil" },
		{ "stealth", typeid(std::nullopt), "AI does not notice you walking around" },
		{ "retard", typeid(std::nullopt), "AI becomes retarded" }
	});

	DWORD pid = System::WaitForWindow(L"HITMAN 2");

	Process process(pid);

	if (!process.Verify("578fef87dc2e3f6f47727f5a99fabf10a31d0059d9c397e858e2a344b960766b"))
	{
		LogError << "Expected Hitman 2 with DX 12 (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infammo"))
	{
		auto bytes = Process::ReadFunction(InfAmmo);

		process.InjectX64(0x1262A2, 2, bytes);
	}

	if (args.Contains("crosshair"))
	{
		process.ChangeBytes(0x125F82,
			ByteStream("F3 44 0F 11 83 0C 08 00 00"), // movss [rbx+0000080C],xmm8
			ByteStream("F3 44 0F 11 8B 0C 08 00 00"));  // movss [rbx+0000080C],xmm9

		// This seems to work also
		//	process.ChangeBytes(0x11ABA7,
		//		ByteStream("F3 44 0F 11 83 0C 08 00 00 F3 0F 58 C6"), // mulss xmm0,[rdi+000003F4]
		//		ByteStream("F3 41 0F 59 C7 90 90 90 90 90 90 90 90")); // mulss xmm0,xmm15
	}

	if (args.Contains("fastfire"))
	{
		process.ChangeBytes(0x12626D,
			ByteStream("FF 83 F0 07 00 00"), // inc [rbx+000007F0]
			ByteStream("FF 8B F0 07 00 00"));  // dec [rbx+000007F0]
	}

	if (args.Contains("lowrecoil"))
	{
		process.ChangeBytes(0x1262EA,
			ByteStream("74 3C"),  // je 3C
			ByteStream("75 3C")); // jne 3C
	}

	if (args.Contains("stealth"))
	{
		process.ChangeBytes(0x4E4C26,
			ByteStream("E8 15 4C E1 FF"),  // call 1402F9840
			ByteStream("90 90 90 90 90")); // nop
	}

	if (args.Contains("retard"))
	{
		process.ChangeBytes(0x32C8A6,
			ByteStream("E8 45 CC 00 00"),  // call 1403394F0
			ByteStream("90 90 90 90 90")); // nop
	}

	process.WairForExit();

	return 0;
}
