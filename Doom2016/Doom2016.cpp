#include "HackLib.hpp"

/*
	NOTE: DOOM has inbuilt cheats which are can be toggled in the console.
	However, the save file is altered after you touch the console key '~'
	and affects the game play e.g. collectibles are no longer valid.
*/

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	int exitCode = 0;

	const CommandLine args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "Infinite ammo" },
		{ "instantcool", typeid(std::nullopt), "Instant weapon cooldown" },
		{ "instantcharge", typeid(std::nullopt), "Instant weapon charge" },
		{ "freeupgrades", typeid(std::nullopt), "Free weapon & gear upgrades" },
	});

	DWORD pid = System::WaitForExe(L"DOOMx64vk.exe");

	Process process(pid);

	if (!process.Verify("139763e94f1a75b5310179f9eeeb8a949a1f53c49acbc722fcfc5dfe7bb6d323"))
	{
		LogError << "Expected Doom using Vulkan API (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	if (args.Contains("infammo"))
	{
		process.ChangeBytes(0xC981E1,
			ByteStream("01 51 38"),
			ByteStream("FF 41 38"));
	}

	if (args.Contains("instantcool"))
	{
		process.ChangeByte(0xCAB583, X86::AddGvEv, X86::SubEvGv);
	}

	if (args.Contains("instantcharge"))
	{
		process.ChangeBytes(0xCC996E,
			ByteStream("8B 08"), // mov ecx,[rax]
			ByteStream("31 C9")); // xor ecx, ecx
	}

	if (args.Contains("freeupgrade"))
	{
		process.ChangeBytes(0xB81A2C,
			ByteStream("89 81 B8 5A 05 00"), // mov [rcx+00055AB8],eax
			ByteStream("FF 81 B8 5A 05 00")); // inc [rcx+00055AB8]
	}

	return exitCode;
}