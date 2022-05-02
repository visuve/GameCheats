#include "HackLib.hpp"

struct Soldier
{
	uint8_t Speed = 0;
	uint8_t HealthA = 0;
	uint8_t HealthB = 0;
	uint8_t Goo = 0; // I do not know what this is
	uint8_t Stamina = 0; // Divided by two
	uint8_t Reactions = 0;
	uint8_t Strength = 0;
	uint8_t Bravery = 0; // Multiplied by ten
	uint8_t PsiEnergy = 0;
	uint8_t PsiAttack = 0;
	uint8_t PsiDefence = 0;

	void MaxOut()
	{
		Speed = 0xFFu;
		HealthA = 0xFFu;
		HealthB = 0xFFu;
		Stamina = 0xC8u;
		Reactions = 0xFFu;
		Bravery = 0x0Au;
		Strength = 0xFFu;
		PsiEnergy = 0xFFu;
		PsiAttack = 0xFFu;
		PsiDefence = 0xFFu;
	}
};

std::ostream& operator << (std::ostream& os, const Soldier& s)
{
	os << "Speed:       " << +s.Speed;
	os << "Health:      " << +s.HealthA << '/' << +s.HealthB;
	os << "Stamina:     " << +(s.Stamina / 2);
	os << "Reactions:   " << +s.Reactions;
	os << "Bravery:     " << uint16_t(s.Bravery) * 10u;
	os << "Strength:    " << +s.Strength;
	os << "Psi-energy:  " << +s.PsiEnergy;
	os << "Psi-attack:  " << +s.PsiAttack;
	os << "Psi-defense: " << +s.PsiDefence;
	return os;
}

std::fstream FindName(std::filesystem::path path, const std::string& name)
{
	uintmax_t fileSize = std::filesystem::file_size(path);

	if (name.length() > 26)
	{
		throw ArgumentException("The name max is 26 characters");
	}

	if (fileSize > 0xA00000)
	{
		throw ArgumentException("The file size is over 10 MiB. What have you done?");
	}

	std::fstream file;
	file.exceptions(std::fstream::failbit | std::fstream::badbit);
	file.open(path, std::ios::in | std::ios::out | std::ios::binary);

	std::string data;
	data.reserve(static_cast<size_t>(fileSize));

	data.assign(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());

	size_t offset = data.find(name);

	if (!offset || offset == std::string::npos)
	{
		throw RangeException(name + " not found");
	}

	file.seekg(offset, std::ios::beg);

	return file;
}

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CmdArgs args(givenArguments,
	{
		{ "infmoney", typeid(std::nullopt), "Infinite money (DOSBox only). Exclusive to other parameters.\n"
			"\t\t\tWarning, very hacky! Make sure you do not have other DOSBox instances running!" },
		{ "path", typeid(std::filesystem::path), "Path to your save game file" },
		{ "patchsoldier", typeid(std::nullopt), "Patch a soldier" },
		{ "patchbiochemist", typeid(std::nullopt), "Patch a biochemist" },
		{ "patchphysicist", typeid(std::nullopt), "Patch a physicist" },
		{ "patchengineer", typeid(std::nullopt), "Patch an engineer" },
		{ "name", typeid(std::string), "The name of the soldier/engineer/scientist" },
	});

	if (args.Contains("infmoney"))
	{
		DWORD pid = System::WaitForExe(L"dosbox.exe");

		Process process(pid);

		if (!process.Verify("c846e94041bef52cc0700a4c8f64449d72880bcb5a85e195030147c3ee8bd319"))
		{
			LogError << "Expected XCOM Apocalypse running in DOSBox v0.7.2";
			System::BeepBurst();
			return ERROR_REVISION_MISMATCH;
		}

		ByteStream bytes;

		bytes << "81 FF CC D4 83 10"; // cmp edi,1083D4CC
		bytes << "75 05"; // jne 5
		bytes << "BB 15 CD 5B 07"; // mov ebx,075BCD15
		bytes << "F7 C7 03 00 00 00"; // test edi,00000003

		process.InjectX86(0xB5B93, 1, bytes);

		return process.WairForExit();
	}

	if (!args.Contains("name") || !args.Contains("path"))
	{
		throw CmdArgs::Exception("All patches require a path and a name to be given", args.Usage());
	}

	std::streamoff offset;

	if (args.Contains("patchsoldier"))
	{
		offset = 58;
	}
	else if (args.Contains("patchbiochemist"))
	{
		offset = 73;
	}
	else if (args.Contains("patchphysicist"))
	{
		offset = 74;
	}
	if (args.Contains("patchengineer"))
	{
		offset = 75;
	}
	else
	{
		throw CmdArgs::Exception("Cannot know what do you want to patch", args.Usage());
	}

	const std::filesystem::path path = args.Value<std::filesystem::path>("path");
	const std::string name = args.Value<std::string>("name");

	if (args.Contains("patchsoldier"))
	{
		std::fstream file = FindName(path, name);

		if (!file)
		{
			throw LogicException("???");
		}

		file.seekg(offset, std::ios::cur);

		Soldier soldier;
		file.read(reinterpret_cast<char*>(&soldier), sizeof(Soldier));

		Log << name << " had:";
		std::cout << soldier << std::endl;

		soldier.MaxOut();

		offset = 0;
		offset -= sizeof(Soldier);

		file.seekp(offset, std::ios::cur);
		file.write(reinterpret_cast<char*>(&soldier), sizeof(Soldier));
		file.flush();

		Log << name << " now has:";
		std::cout << soldier << std::endl;

		Log << "\n... KTHXBYE.";
	}

	std::fstream file = FindName(path, name);

	if (!file)
	{
		throw LogicException("???");
	}

	file.seekg(offset, std::ios::cur);

	uint8_t value = 0;
	file >> value;
	Log << name << " had skill of: " << +value;

	value = 0xFF;

	offset = 0;
	offset -= sizeof(uint8_t);

	file.seekp(offset, std::ios::cur);

	file << value;

	file.flush();

	Log << name << " now has skill of: " << +value;

	Log << "\n... KTHXBYE.";

	return 0;
}
