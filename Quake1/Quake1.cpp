#include "HackLib.hpp"

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
		Items = static_cast<float>(uint32_t(Items) | items);
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
			{ L"infammo", typeid(std::nullopt), L"255 ammo always" }
		});

		DWORD pid = System::Instance().WaitForExe(L"Quake_x64_steam.exe");

		Process process(pid);

		if (!process.Verify("39d0a522918f078425bda90d43292e9bb83866d442059deb5be75ae8f4f8109a"))
		{
			LogError << "Expected Quake 1 Remake v1.0.5036 (Steam)";
			return ERROR_REVISION_MISMATCH;
		}

		if (args.Contains(L"infammo"))
		{
			ByteStream stream;

			// Orig
			stream << "48 63 14 B3"; // movsxd rdx,dword ptr [rbx+rsi*4]
			stream << "48 A1" << process.Address(0x18DB3D0); // mov rax,[1418DB3D0]
			stream << "8B 0C BB"; // mov ecx,[rbx+rdi*4]

			// If rdx hits the spot, lets set 255 to ecx
			stream << "48 81 FA AC 05 00 00"; // cmp rdx, 000005AC
			stream << "74 26"; // je
			stream << "48 81 FA B0 05 00 00"; // cmp rdx, 000005B0
			stream << "74 1D"; // je
			stream << "48 81 FA B4 05 00 00"; // cmp rdx, 000005B4
			stream << "74 14"; // je
			stream << "48 81 FA B8 05 00 00"; // cmp rdx, 000005B8
			stream << "74 0B"; // je
			stream << "48 81 FA BC 05 00 00"; // cmp rdx, 000005BC
			stream << "74 02"; // je
			stream << "EB 05"; // jmp 5
			stream << "B9 00 00 7F 43"; // mov ecx, 437F0000 (float 255)
			stream << "90"; // nop

			// Orig
			stream << "89 0C 02"; // mov [rdx+rax],ecx <- the fucker from the original code

			process.InjectX64(0x1CBE71, 3, stream);

			return process.WairForExit();
		}
	}
	catch (const CmdArgs::Exception& e)
	{
		LogError << '\n' << e.what() << "!\n";
		std::wcerr << e.Usage();
		return ERROR_BAD_ARGUMENTS;
	}
	catch (const std::system_error& e)
	{
		LogError << e.what();
		return e.code().value();
	}
	catch (const std::exception& e)
	{
		LogError << e.what();
		return ERROR_PROCESS_ABORTED;
	}

	return 0;
}
