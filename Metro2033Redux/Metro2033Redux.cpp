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
		// Reload related
		// Original: mov [rcx+000004F4],edx
		process.Fill(0x2B2729, 0x2B272F, X86::Nop);

		// Shooting itself
		// Original: mov [rsi+00000448], r14d
		process.Fill(0x2C0B2F, 0x2C0B36, X86::Nop);

	}

	return 0;
}
