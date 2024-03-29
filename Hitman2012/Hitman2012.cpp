#include "HackLib.hpp"

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CommandLine args(givenArguments,
	{
		{ "infstealth", typeid(std::nullopt), "Infinite stealth" },
		{ "infhealth", typeid(std::nullopt), "Infinite health" },
		{ "infammo", typeid(std::nullopt), "Infinite ammo" },
	});

	DWORD pid = System::WaitForExe(L"HMA.exe");

	Process process(pid);

	if (!process.Verify("58607bbc54bd577c752ee0f04a8cfec6873855639c43d4a24bd2d1b6c693e9da"))
	{
		LogError << "Expected Hitman Absolution v1.0.447 (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infstealth"))
	{
		process.Fill(0x60A926, 0x60A929, X86::Nop);
	}

	if (args.Contains("infhealth"))
	{
		process.Fill(0x4082E1, 0x4082E3, X86::Nop);
	}

	if (args.Contains("infammo"))
	{
		process.ChangeByte(0x5ECEAD, X86::DecEax, X86::IncEax);
	}

	return 0;
}
