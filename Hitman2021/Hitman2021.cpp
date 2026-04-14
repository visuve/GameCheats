#include "HackLib.hpp"

extern "C" void InfAmmo();

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CommandLine args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "Infinite ammunition" },
		{ "crosshair", typeid(std::nullopt), "The crosshair does not spread when shooting" },
		{ "fastfire", typeid(std::nullopt), "Semi-automatic guns can fire faster" },
		{ "lowrecoil", typeid(std::nullopt), "Low recoil" },
		{ "godmode", typeid(std::nullopt), "Become invulnerable" },
		{ "stealth", typeid(std::nullopt), "Become invisible" },
		{ "onehitkill", typeid(std::nullopt), "One shot kills an NPC" },
	});

	DWORD pid = System::WaitForWindow(L"HITMAN 3");

	Process process(pid);

	if (!process.Verify("b1923a6aeed196609c8e3ac2fa59c4b6aa3bad8ff5a4094da7d30b40879a84c5"))
	{
		LogError << "Expected Hitman 3 (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infammo"))
	{
		auto bytes = Process::ReadFunction(InfAmmo);
		process.WriteBytes(0x48CB7D, bytes);
	}

	if (args.Contains("crosshair"))
	{
		process.ChangeBytes(0x48F7D1,
			ByteStream("F3 0F 11 BF 4C 09 00 00"), // movss [rdi+0000094C],xmm7
			ByteStream("0F 57 FF 90 90 90 90 90")); // xorps xmm7,xmm7; nop; nop; nop; nop; nop
	}

	if (args.Contains("fastfire"))
	{
		process.ChangeBytes(0x48FA90,
			ByteStream("FF 87 30 09 00 00"), // inc dword ptr [rdi+00000930]
			ByteStream("FF 8F 30 09 00 00")); // dec dword ptr [rdi+00000930]
	}

	if (args.Contains("lowrecoil"))
	{
		//process.ChangeBytes(0x48FD51,
		//	ByteStream("74 3C"),  // je 3C
		//	ByteStream("75 3C")); // jne 3C

		process.ChangeBytes(0x48FD51,
			ByteStream("74 3C"),  // je 3C
			ByteStream("EB 3C")); // jmp 3C

		process.ChangeBytes(0x6EFB18,
			ByteStream("F3 0F 5C F0"), // subss xmm6, xmm0
			ByteStream("0F 57 F6 90")); // xorps xmm6, xmm6; nop
	}

	if (args.Contains("godmode"))
	{
		//	process.ChangeBytes(0x141A15,
		//		ByteStream("F3 0F 10 55 E3"), // movss xmm2, [rbp-1D]
		//		ByteStream("0F 57 D2 90 90")); // xorps xmm2,xmm2; nop; nop

		process.ChangeBytes(0x2D7A38,
			ByteStream("E8 63 98 E6 FF"), // call hitman3.exe + 0x1412A0
			ByteStream("B8 00 00 00 00")); // mov eax, 0; nop; nop; nop; nop
	}

	if (args.Contains("stealth"))
	{
		process.ChangeBytes(0x18BB8D,
			ByteStream("0F 84 2E 06 00 00"), // je 62E
			ByteStream("90 E9 2E 06 00 00")); // nop; jmp 62E
	}

	if (args.Contains("onehitkill"))
	{
		process.ChangeBytes(0xBCE7B,
			ByteStream("0F 28 C8 F3 0F 58"), // addss xmm1, xmm0; addss xmm0, [rax]
			ByteStream("0F 57 C9 EB 06 90")); // xorps xmm1, xmm1; jmp 6; nop; nop; nop; nop; nop

		process.ChangeBytes(0xBCEBA,
			ByteStream("0F 28 C8 F3 0F 58"), // addss xmm1, xmm0; addss xmm0, [rax]
			ByteStream("0F 57 C9 EB 06 90")); // xorps xmm1, xmm1; jmp 6; nop; nop; nop; nop; nop
	}

	return 0;
}
