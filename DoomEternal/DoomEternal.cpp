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
		{ "infammo", typeid(std::nullopt), "Infinite ammo" },
		{ "freeupgrades", typeid(std::nullopt), "Free weapon & gear upgrades" },
		{ "coollauncher", typeid(std::nullopt), "No cooldown for equipment launcher (grenades & flame)" },
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
		process.ChangeBytes(0x1D24A03,
			ByteStream("89 7B 40"), // mov [rbx+40], edi
			ByteStream("FF C7 90")); // inc edi; nop

		process.ChangeBytes(0x1D24A31,
			ByteStream("89 7B 40"), // mov [rbx+40], edi
			ByteStream("90 90 90")); // mov [rbx+40], edi
	}

	if (args.Contains("freeupgrades"))
	{
		process.ChangeBytes(0x1A4899F,
			ByteStream("44 01 84 B1 84 CD 04 00"), // add [rcx+rsi*4+0004CD84],r8d
			ByteStream("FF 84 B1 84 CD 04 00 90")); // inc [rcx+rsi*4+0004CD84]; nop
	}

	if (args.Contains("coollauncher"))
	{
		process.ChangeBytes(0xA5F45BE,
			ByteStream("FF 0C C7"), // dec[rdi + rax * 8]
			ByteStream("FF 04 C7")); // inc[rdi + rax * 8]

		process.ChangeBytes(0xA5F4654,
			ByteStream("FF 8F E0 6F 00 00"), // dec [rdi+00006FE0]
			ByteStream("FF 87 E0 6F 00 00")); // inc [rdi+00006FE0]
	}

	return exitCode;
}