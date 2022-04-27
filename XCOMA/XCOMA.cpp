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
	os << "Speed:       " << +s.Speed ;
	os << "Health:      " << +s.HealthA << '/' << +s.HealthB ;
	os << "Stamina:     " << +(s.Stamina / 2) ;
	os << "Reactions:   " << +s.Reactions ;
	os << "Bravery:     " << uint16_t(s.Bravery) * 10u ;
	os << "Strength:    " << +s.Strength ;
	os << "Psi-energy:  " << +s.PsiEnergy ;
	os << "Psi-attack:  " << +s.PsiAttack ;
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

		const std::filesystem::path path = args.Value<std::filesystem::path>(L"path");
		const std::string name = StrConvert::ToUtf8(args.Value<std::wstring>(L"name"));

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

		Log << "\n... KTHXBYE." ;

	}
	catch (const CmdArgs::Exception& e)
	{
		LogError << e.what() << "\n";
		std::wcerr << e.Usage();
		return ERROR_BAD_ARGUMENTS;
	}
	catch (const std::system_error& e)
	{
		LogError << e.what();
		System::BeepBurst();
		return e.code().value();
	}
	catch (const std::exception& e)
	{
		LogError << e.what();
		System::BeepBurst();
		return ERROR_PROCESS_ABORTED;
	}

	return 0;
}
