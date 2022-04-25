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

int wmain(int argc, wchar_t** argv)
{
	try
	{
		const CmdArgs args(argc, argv,
		{
			{ L"infmoney", typeid(std::nullopt), L"Infinite money (DOSBox only). Exclusive to other parameters.\n"
				L"\t\t\tWarning, very hacky! Make sure you do not have other DOSBox instances running!" },
			{ L"path", typeid(std::filesystem::path), L"Path to your save game file" },
			{ L"patchsoldier", typeid(std::nullopt), L"Patch a soldier" },
			{ L"patchbiochemist", typeid(std::nullopt), L"Patch a biochemist" },
			{ L"patchphysicist", typeid(std::nullopt), L"Patch a physicist" },
			{ L"patchengineer", typeid(std::nullopt), L"Patch an engineer" },
			{ L"name", typeid(std::wstring), L"The name of the soldier/engineer/scientist" },
		});

		if (args.Contains(L"infmoney"))
		{
			DWORD pid = System::Instance().WaitForExe(L"dosbox.exe");

			Process process(pid);

			ByteStream bytes;

			bytes << "81 FF CC D4 83 10"; // cmp edi,1083D4CC
			bytes << "75 05"; // jne 5
			bytes << "BB 15 CD 5B 07"; // mov ebx,075BCD15
			bytes << "F7 C7 03 00 00 00"; // test edi,00000003

			process.InjectX86(0xB5B93, 1, bytes);

			return process.WairForExit();
		}

		if (!args.Contains(L"name") || !args.Contains(L"path"))
		{
			throw CmdArgs::Exception("All patches require a path and a name to be given", args.Usage());
		}

		std::streamoff offset;

		if (args.Contains(L"patchsoldier"))
		{
			offset = 58;
		}
		else if (args.Contains(L"patchbiochemist"))
		{
			offset = 73;
		}
		else if (args.Contains(L"patchphysicist"))
		{
			offset = 74;
		}
		if (args.Contains(L"patchengineer"))
		{
			offset = 75;
		}
		else
		{
			throw CmdArgs::Exception("Cannot know what do you want to patch", args.Usage());
		}

		const std::filesystem::path path = args.Get<std::filesystem::path>(L"path");
		const std::string name = StrConvert::ToUtf8(args.Get<std::wstring>(L"name"));

		if (args.Contains(L"patchsoldier"))
		{
			std::fstream file = FindName(path, name);

			if (!file)
			{
				throw LogicException("???");
			}
			
			file.seekg(offset, std::ios::cur);

			Soldier soldier;
			file.read(reinterpret_cast<char*>(&soldier), sizeof(Soldier));

			std::cout << name << " had:" << std::endl;
			
			std::cout << soldier << std::endl << std::endl;

			soldier.MaxOut();

			offset = 0;
			offset -= sizeof(Soldier);

			file.seekp(offset, std::ios::cur);
			file.write(reinterpret_cast<char*>(&soldier), sizeof(Soldier));
			file.flush();

			std::cout << name << " now has:" << std::endl;
			std::cout << soldier << std::endl;

			std::cout << std::endl << "... KTHXBYE." << std::endl;
		}

		std::fstream file = FindName(path, name);

		if (!file)
		{
			throw LogicException("???");
		}

		file.seekg(offset, std::ios::cur);

		uint8_t value = 0;
		file >> value;
		std::cout << name << " had skill of: " << +value << std::endl;

		value = 0xFF;

		offset = 0;
		offset -= sizeof(uint8_t);

		file.seekp(offset, std::ios::cur);

		file << value;

		file.flush();

		std::cout << name << " now has skill of: " << +value << std::endl;

		std::cout << std::endl << "... KTHXBYE." << std::endl;

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
