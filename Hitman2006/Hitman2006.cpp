#include "HackLib.hpp"

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CmdArgs args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "Never decreasing ammunition" },
		{ "infhealth", typeid(std::nullopt), "Never decreasing health" }
	});

	DWORD pid = System::WaitForWindow(L"Hitman Blood Money");

	Process process(pid);

	if (!process.Verify("b45bf59665f98b6547152218f33a0fe006836290f004960a49c37918f22d2713"))
	{
		LogError << "Expected Hitman Blood Money v1.2 (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infhealth"))
	{
		process.Fill(0x1FB973, 0x1FB977, X86::Nop);
	}

	if (args.Contains("infammo"))
	{
		ByteStream code;

		// Stolen
		code << "8B 8E 94 00 00 00"; // mov ecx,[esi+00000094]
		code << "8B 86 A8 00 00 00"; // mov eax,[esi+000000A8]

		// New
		code << "81 FC FC F5 19 00"; // cmp esp, 0019F5FC
		code << "74 0B"; // je 11
		code << "81 FC 3C F7 19 00"; // cmp esp, 0019F73C
		code << "74 03"; // je 3
		code << "49"; // dec ecx
		code << "EB 01"; // jmp 1
		code << "41"; // inc ecx
		code << "90"; // nop

		process.InjectX86(0x1140DB, 8, code);
		return process.WairForExit();
	}

	return 0;
}
