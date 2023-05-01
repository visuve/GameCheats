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
		{ "health", typeid(float), "Set current and max health. Default 100.0." },
		{ "armor", typeid(float), "Set current and max armor. Default 50.0." },
		{ "damage", typeid(float), "Damage scale. Default 1.0. Note: does not appear to be linear." }
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

		Log << "Infinite ammo applied";
	}

	if (args.Contains("freeupgrades"))
	{
		process.ChangeBytes(0x1A4899F,
			ByteStream("44 01 84 B1 84 CD 04 00"), // add [rcx+rsi*4+0004CD84],r8d
			ByteStream("FF 84 B1 84 CD 04 00 90")); // inc [rcx+rsi*4+0004CD84]; nop

		Log << "Free upgrades applied";
	}

	if (args.Contains("coollauncher"))
	{
		process.ChangeBytes(0xA5F45BE,
			ByteStream("FF 0C C7"), // dec[rdi + rax * 8]
			ByteStream("FF 04 C7")); // inc[rdi + rax * 8]

		process.ChangeBytes(0xA5F4654,
			ByteStream("FF 8F E0 6F 00 00"), // dec [rdi+00006FE0]
			ByteStream("FF 87 E0 6F 00 00")); // inc [rdi+00006FE0]

		Log << "Cool launcher applied";
	}

	Pointer doomGuy;

	if (args.Contains("health") || args.Contains("armor"))
	{
		// All of the above appear to work, but I just picked the nicest
		// doomGuy = process.ResolvePointer(0x6BC2270, 0x38, 0x5DCu, 0x58u);
		// doomGuy = process.ResolvePointer(0x6BC2270, 0x1F0, 0x52Cu, 0x58u);
		doomGuy = process.ResolvePointer(0x6BC2270, 0x0u, 0x20u, 0x58u);
		// doomGuy = process.ResolvePointer(0x6BC2270, 0x400, 0x5DCu, 0x58u);
		// doomGuy = process.ResolvePointer(0x6BC2270, 0x1F0, 0x82Cu, 0x58u);
		LogVariableHex(doomGuy.Value());
	}

	if (args.Contains("health"))
	{
		float wantedHealth = args.Value<float>("health", 100.0f);

		Pointer maxHealthPtr = doomGuy + 0x38u;
		Pointer healthPtr = doomGuy + 0x44u;

		LogVariableHex(maxHealthPtr.Value());
		LogVariableHex(healthPtr.Value());

		float maxHealth = process.Read<float>(maxHealthPtr);
		float health = process.Read<float>(healthPtr);

		LogDebug << "Before:";
		LogVariable(maxHealth);
		LogVariable(health);

		process.Write(maxHealthPtr, wantedHealth);
		process.Write(healthPtr, wantedHealth);

		maxHealth = process.Read<float>(maxHealthPtr);
		health = process.Read<float>(healthPtr);

		LogDebug << "After:";
		LogVariable(maxHealth);
		LogVariable(health);

		Log << "Health updated";
	}

	if (args.Contains("armor"))
	{
		float wantedArmor = args.Value<float>("armor", 50.0f);

		Pointer maxArmorPtr = doomGuy + 0xE8;
		Pointer armorPtr = doomGuy + 0xF4;

		LogVariableHex(maxArmorPtr.Value());
		LogVariableHex(armorPtr.Value());

		float maxArmor = process.Read<float>(maxArmorPtr);
		float armor = process.Read<float>(armorPtr);

		LogDebug << "Before:";
		LogVariable(maxArmor);
		LogVariable(armor);

		process.Write(maxArmorPtr, wantedArmor);
		process.Write(armorPtr, wantedArmor);

		maxArmor = process.Read<float>(maxArmorPtr);
		armor = process.Read<float>(armorPtr);

		LogDebug << "After:";
		LogVariable(maxArmor);
		LogVariable(armor);

		Log << "Armor updated";
	}

	if (args.Contains("damage"))
	{
		float wantedDamage = args.Value<float>("damage", 1.0f);
		Pointer damagePtr = process.ResolvePointer(0x6B81080, 0xCu);
		float damage = process.Read<float>(damagePtr);

		LogDebug << "Before:";
		LogVariable(damage);

		process.Write(damagePtr, wantedDamage);
		damage = process.Read<float>(damagePtr);

		LogDebug << "After:";
		LogVariable(damage);
	}

	return exitCode;
}