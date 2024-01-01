#include "HackLib.hpp"

/*
	NOTE: Doom Eternal has inbuilt cheats which are can be toggled in the console.
	However, the console commands are locked and require special tools.
	I might try to reverse them later and put here...
*/

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	int exitCode = 0;

	const CommandLine args(givenArguments,
	{
		{ "infammo", typeid(std::nullopt), "Infinite ammo" },
		{ "freeupgrades", typeid(std::nullopt), "Free weapon & gear upgrades. Note: do not use before third level (Cultist Base)..." },
		{ "coollauncher", typeid(std::nullopt), "No cooldown for equipment launcher (grenades & flame)" },
		{ "damage", typeid(float), "Damage scale. Default 1.0. Note: does not appear to be linear." },
		{ "health", typeid(float), "Set current and max health. Default 100.0. Note: a map needs to be loaded!" },
		{ "armor", typeid(float), "Set current and max armor. Default 50.0. Note: a map needs to be loaded!" },
		{ "coolguns", typeid(std::nullopt), "No cooldown for weapons. NOTE: Does not work for sticky bombs and micro-missiles glitch." }
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

		LogInfo << "Infinite ammo applied";
	}

	if (args.Contains("freeupgrades"))
	{
		process.ChangeBytes(0x1A4899F,
			ByteStream("44 01 84 B1 84 CD 04 00"), // add [rcx+rsi*4+0004CD84],r8d
			ByteStream("FF 84 B1 84 CD 04 00 90")); // inc [rcx+rsi*4+0004CD84]; nop

		LogInfo << "Free upgrades applied";
	}

	if (args.Contains("coollauncher"))
	{
		process.ChangeBytes(0xA5F45BE,
			ByteStream("FF 0C C7"), // dec[rdi + rax * 8]
			ByteStream("FF 04 C7")); // inc[rdi + rax * 8]

		process.ChangeBytes(0xA5F4654,
			ByteStream("FF 8F E0 6F 00 00"), // dec [rdi+00006FE0]
			ByteStream("FF 87 E0 6F 00 00")); // inc [rdi+00006FE0]

		LogInfo << "Cool launcher applied";
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

		LogInfo << "More damage applied";
	}

	if (args.Contains("coolguns"))
	{
		process.ChangeBytes(0x1D407F9,
			ByteStream("F3 41 0F 11 B7 68 31 00 00"), // movss [r15+00003168],xmm6
			ByteStream("F3 45 0F 11 8F 68 31 00 00")); // movss [r15+00003168],xmm9

		Pointer compare = process.ResolvePointer(0x6BF36F0, 0x8);

		process.Write(compare, uint64_t(1));
	}

	Pointer doomGuy;

	if (args.Contains("health") || args.Contains("armor"))
	{
		// All of the below appear to work, but I just picked the nicest
		// doomGuy = process.ResolvePointer(0x6BC2270, 0x38u, 0x5DCu, 0x58u);
		// doomGuy = process.ResolvePointer(0x6BC2270, 0x1F0u, 0x52Cu, 0x58u);
		doomGuy = process.ResolvePointer(0x6BC2270, 0x0u, 0x20u, 0x58u);
		// doomGuy = process.ResolvePointer(0x6BC2270, 0x400u, 0x5DCu, 0x58u);
		// doomGuy = process.ResolvePointer(0x6BC2270, 0x1F0u, 0x82Cu, 0x58u);
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

		LogInfo << "Health updated";
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

		LogInfo << "Armor updated";
	}

	return exitCode;
}