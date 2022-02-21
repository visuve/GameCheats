#include "Quake1-PCH.hpp"

/*
	Infinite ammo in Quake 1 Remake 
	Tested with version 1.0.4831 (Steam), SHA-256
	094cceaabbbc3fbc794e70de97e107da7a6ef8ea818372a598908ecc8bca71f6
*/

enum Items : uint32_t
{
	Axe = 4096,
	Shotgun = 1,
	SuperShotgun = 2,
	NailGun = 4,
	SuperNailGun = 8,
	GrenadeLauncher = 16,
	RocketLauncher = 32,
	LightningGun = 64
};

constexpr uint32_t AllItems = 
	Axe | Shotgun | SuperShotgun | NailGun | SuperNailGun | GrenadeLauncher | RocketLauncher | LightningGun;

struct Player
{
	float CurrentAmmo = 0;
	float Shells = 0;
	float Nails = 0;
	float Rockets = 0;
	float Cells = 0;
	float Items = 0;

	void SetAmmo(float value)
	{
		CurrentAmmo = value;
		Shells = value;
		Nails = value;
		Rockets = value;
		Cells = value;
	}

	void SetItems(uint32_t items)
	{
		Items = (uint32_t(Items) | items);
	}
};

std::ostream& operator << (std::ostream& os, const Player& p)
{
	os << "Current ammo: " << p.CurrentAmmo << std::endl;
	os << "Shells:       " << p.Shells << std::endl;
	os << "Nails:        " << p.Nails << std::endl;
	os << "Rockets:      " << p.Rockets << std::endl;
	os << "Cells:        " << p.Cells << std::endl;
	os << "Items:        " << p.Items;
	return os;
}

int wmain(int argc, wchar_t** argv)
{
	if (argc <= 1)
	{
		return ERROR_BAD_ARGUMENTS;
	}

	try
	{
		const CmdArgs args(argc, argv);

		if (args.Contains(L"infammo"))
		{
			Process process(L"Quake_x64_steam.exe");

			Pointer ammoPtr = process.ResolvePointer(0x018E2950, 0x88);
			Player player = process.Read<Player>(ammoPtr);
			
			std::cout << "Before:" << std::endl;
			std::cout << player << std::endl;

			player.SetAmmo(std::numeric_limits<float>::infinity());
			player.SetItems(AllItems);

			std::cout << "\nAfter:" << std::endl;
			std::cout << player << std::endl;

			process.Write(ammoPtr, player);
		}

	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
