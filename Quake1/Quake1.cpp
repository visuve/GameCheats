#include "../Mega.pch"

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
	try
	{
		const CmdArgs args(argc, argv,
		{
			{ L"giveammo", L"Set 255 ammo for all weapons" },
			{ L"infammo", L"255 ammo always" }
		});

		Process process(L"Quake_x64_steam.exe");

		if (!process.Verify("094cceaabbbc3fbc794e70de97e107da7a6ef8ea818372a598908ecc8bca71f6"))
		{
			std::cerr << "Expected Quake 1 Remake v1.0.4831 (Steam)" << std::endl;
			return ERROR_REVISION_MISMATCH;
		}

		if (args.Contains(L"giveammo"))
		{
			Pointer ammoPtr = process.ResolvePointer(0x018E2950, 0x88);
			Player player = process.Read<Player>(ammoPtr);

			std::cout << "Before:" << std::endl;
			std::cout << player << std::endl;

			player.SetAmmo(0xFF);
			player.SetItems(AllItems);

			std::cout << "\nAfter:" << std::endl;
			std::cout << player << std::endl;

			process.Write(ammoPtr, player);
		}

		if (args.Contains(L"infammo"))
		{
			ByteStream stream;

			// Orig
			stream << 0x48 << 0x63 << 0x14 << 0xB3;
			stream << 0x48 << 0xA1 << process.Address(0x18BEEB0);
			stream << 0x8B << 0x0C << 0xBB;

			// Hax
			stream << 0x48 << 0x81 << 0xFA << 0xA8 << 0x05 << 0x00 << 0x00; // 48 81 FA A8050000 - cmp rdx, 000005A8
			stream << 0X7C << 0X0E; // 7C - jl
			stream << 0x48 << 0x81 << 0xFA << 0xB4 << 0x05 << 0x00 << 0x00; // 48 81 FA B4050000 - cmp rdx, 000005B4
			stream << 0X7F << 0X05; // 7F - jg
			stream << 0xB9 << 0x00 << 0x00 << 0x7F << 0x43; // B9 00007E43 - mov ecx, 437F0000

			stream << 0x89 << 0x0C << 0x02; // mov [rdx+rax],ecx <- the fucker from the original code

			process.InjectX64(0x1C7BA1, 3, stream);

			process.WairForExit();
		}
	}
	catch (const CmdArgs::MissingArguments& e)
	{
		std::cerr << '\n' << e.what() << "!\n" << std::endl;
		std::wcerr << e.Usage() << std::endl;
		return ERROR_BAD_ARGUMENTS;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
