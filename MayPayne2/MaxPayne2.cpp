#include "HackLib.hpp"

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CmdArgs args(givenArguments,
	{
		{ "reloadadds", typeid(std::nullopt), "Reloading adds to total ammo" },
		{ "infammo", typeid(std::nullopt), "Ammunition is never reduced" }
	});

	DWORD pid = System::WaitForExe(L"maxpayne2.exe");

	Process process(pid);

	if (!process.Verify("75b46ceaaaecd44173358654f67abd20057fd4298fc6a0d74437c0c884328fc3"))
	{
		LogError << "Expected Max Payne 2 - The Fall of Max of Payne (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("reloadadds") || args.Contains("infammo"))
	{
		Pointer reloadPtr = process.Address(L"X_GameObjectsMFC.dll", 0x7B71D);
		process.ChangeByte(reloadPtr, X86::SubGvEv, X86::AddGvEv);
	}
		
	if (args.Contains("infammo"))
	{
		Pointer ammoPtr = process.Address(L"X_GameObjectsMFC.dll", 0x7D265);
		process.ChangeByte(ammoPtr, X86::DecEcx, X86::Nop);
	}

	return 0;
}
