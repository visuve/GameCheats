#include "HackLib.hpp"

/*
	NOTE: Crysis has inbuilt cheats, which are relatively easy to toggle.
	Just pass -devmode -epicapp=Coot -epicenv=Prod to the application and
	you can access the console via '~' key.


	I'm just a lazy bastard and I do not want to write them each time I run the game.
	Hence these hacks. Apply the cheats once a level has been loaded and have fun.
*/

template<typename T>
void Cheat(const CmdArgs& args, std::string_view key, const Process& process, size_t offset)
{
	T wanted = args.Value<T>(key);

	Pointer ptr = process.ResolvePointer(0x088480F8, offset);

	T actual = process.Read<T>(ptr);

	process.Write<T>(ptr, wanted);

	LogInfo << key << actual << "->" << wanted;
}

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	int exitCode = 0;

	const CmdArgs args(givenArguments,
	{
		{ "i_unlimitedammo", typeid(int32_t), "Unlimited ammo" },
		{ "g_suitRecoilEnergyCost", typeid(float), "Recoil energy cost" },
		{ "g_suitSpeedEnergyConsumption", typeid(float), "Speed energy consumption" },
		{ "g_playerSuitEnergyRechargeTime", typeid(float), "Energy recharge time" },
		{ "g_playerSuitEnergyRechargeTimeArmor", typeid(float), "Energy recharge time armor" },
		{ "g_playerSuitHealthRegenDelay", typeid(float), "Health regen delay" },
		{ "g_playerSuitEnergyRechargeDelay", typeid(float), "Energy recharge delay" },
		{ "g_playerSuitHealthRegenTime", typeid(float), "Health regen time" },
		{ "g_playerSuitHealthRegenTimeMoving", typeid(float), "Health regen time while moving" },
		{ "g_playerSuitArmorModeHealthRegenTime", typeid(float), "Health regen time while armoured" },
	});

	DWORD pid = System::WaitForExe(L"CrysisRemastered.exe");

	Process process(pid);

	if (!process.Verify("336c95e1c2d163519685d7813fc7d5c141d79b6d0eabcef64335930feb8b1fd1"))
	{
		LogError << "Expected Crysis Remastered (Steam)";
		System::BeepBurst();
		return ERROR_REVISION_MISMATCH;
	}

	process.WaitForIdle();
	System::BeepUp();

	const std::tuple<std::string_view, size_t, std::type_index> cheats[] =
	{
		{ "i_unlimitedammo", 0x1CC, typeid(int32_t) },
		{ "g_suitRecoilEnergyCost", 0x25C, typeid(float) },
		{ "g_suitSpeedEnergyConsumption", 0x298, typeid(float) },
		{ "g_playerSuitEnergyRechargeTime", 0x2D4, typeid(float) },
		{ "g_playerSuitEnergyRechargeTimeArmor", 0x2D8, typeid(float) },
		{ "g_playerSuitHealthRegenDelay", 0x2F0, typeid(float) },
		{ "g_playerSuitEnergyRechargeDelay", 0x2F0, typeid(float) }, // wonder why they are the same...
		{ "g_playerSuitHealthRegenTime", 0x2F8, typeid(float) },
		{ "g_playerSuitHealthRegenTimeMoving", 0x2FC, typeid(float) },
		{ "g_playerSuitArmorModeHealthRegenTime", 0x300, typeid(float) }
	};

	for (auto [key, offset, type] : cheats)
	{
		if (!args.Contains(key))
		{
			continue;
		}

		if (type == typeid(int32_t))
		{
			Cheat<int32_t>(args, key, process, offset);
		}
		else if (type == typeid(float))
		{
			Cheat<float>(args, key, process, offset);
		}
		else
		{
			throw ArgumentException("Unsupported type");
		}
	}

	return exitCode;
}
