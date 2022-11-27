#include "HackLib.hpp"

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CmdArgs args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "Ammunition is never reduced" }
	});

	DWORD pid = System::WaitForWindow(L"Metro Redux");

	Process process(pid);

	if (!process.Verify("183ef65212e351c55a2832c3f3c8b04616b153697913d33b1e27683163f14e15"))
	{
		LogError << "Expected Metro 2033 Redux (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infammo"))
	{
		// Original: mov [rcx+000004F4],edx
		process.Fill(0x2B2729, 0x2B272F, X86::Nop);

		// NOTE: a NOP approach does not work when picking up new guns
		// Original: mov [rsi+00000448], r14d
		// process.Fill(0x2C0B2F, 0x2C0B36, X86::Nop);

		// There is extra space, hence I do not need to allocate new memory
		ByteStream hack;
		hack << "41 BE FF 00 00 00"; // mov r14d,000000FF
		hack << "44 89 B6 48 04 00 00"; // mov [rsi+00000448],r14d <- stolen
		hack << "EB C2";
		process.WriteBytes(0x2C0B60, hack);

		ByteStream jump;
		jump << "EB 2F 90 90 90 90 90";

		process.WriteBytes(0x2C0B2F, jump);
	}

	return 0;
}
