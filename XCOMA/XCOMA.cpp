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
	os << "Speed:       " << +s.Speed << std::endl;
	os << "Health:      " << +s.HealthA << '/' << +s.HealthB << std::endl;
	os << "Stamina:     " << +(s.Stamina / 2) << std::endl;
	os << "Reactions:   " << +s.Reactions << std::endl;
	os << "Bravery:     " << uint16_t(s.Bravery) * 10u << std::endl;
	os << "Strength:    " << +s.Strength << std::endl;
	os << "Psi-energy:  " << +s.PsiEnergy << std::endl;
	os << "Psi-attack:  " << +s.PsiAttack << std::endl;
	os << "Psi-defense: " << +s.PsiDefence;
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
		const CmdArgs args(argc, argv,
		{
			{ L"patchsoldier", typeid(std::filesystem::path), L"Patch a soldier" },
			{ L"name", typeid(std::wstring), L"The name of the soldier" }
		});

		if (args.Contains(L"patchsoldier") && args.Contains(L"name"))
		{
			std::filesystem::path path = args.Get<std::filesystem::path>(L"patchsoldier");
			uintmax_t fileSize = std::filesystem::file_size(path);

			std::string name = StrConvert::ToUtf8(args.Get<std::wstring>(L"name"));

			if (name.length() > 26)
			{
				std::cerr << "The name max is 26 characters" << std::endl;
				return -1;
			}

			if (fileSize > 0xA00000)
			{
				std::cerr << "The file size is over 10 MiB. What have you done?" << std::endl;
				return -1;
			}

			// Include the nulls to be safe when searching as the names are 26 bytes
			name.resize(26);

			std::fstream file;
			file.exceptions(std::fstream::failbit | std::fstream::badbit);
			file.open(path, std::ios::in | std::ios::out | std::ios::binary);

			std::string data;
			data.reserve(static_cast<size_t>(fileSize));

			data.assign(
				(std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());

			size_t offset = data.find(name);

			if (!offset)
			{
				std::cerr << "Could not find " << name;
				return -1;
			}

			offset += 58;

			file.seekg(offset, std::ios::beg);

			Soldier soldier;
			file.read(reinterpret_cast<char*>(&soldier), sizeof(soldier));

			std::cout << name << " had:" << std::endl;
			
			std::cout << soldier << std::endl << std::endl;

			soldier.MaxOut();

			file.seekp(offset, std::ios::beg);
			file.write(reinterpret_cast<char*>(&soldier), sizeof(soldier));

			std::cout << name << " now has:" << std::endl;
			std::cout << soldier << std::endl;

			std::cout << std::endl << "... KTHXBYE." << std::endl;
		}
	}
	catch (const CmdArgs::Exception& e)
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
