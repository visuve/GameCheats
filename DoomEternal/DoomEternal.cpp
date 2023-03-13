#include "HackLib.hpp"

/*
	NOTE: Doom Eternal has inbuilt cheats which are can be toggled in the console.
	However, the console commands are locked and require special tools.
	I might try to reverse them later and put here...
*/

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	int exitCode = 0;

	const CmdArgs args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "Infinite ammo" }
	});

	DWORD pid = System::WaitForExe(L"DOOMEternalx64vk.exe");

	Process process(pid);

	if (!process.Verify("84d6d3d01855fa5900f7ff8e452a14d9316ebc459bafdc4c4480831b537ff375"))
	{
		LogError << "Expected Doom Eternal (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infammo"))
	{
		process.ChangeBytes(0x1D24A31,
			ByteStream("89 7B 40"), // mov [rbx+40], edi
			ByteStream("FF 43 40")); // inc [rbx+40]
	}

	return exitCode;
}