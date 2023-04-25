#include "HackLib.hpp"

/*
	NOTES:
	1. I would not install the game using the installer which is not signed and prompts for admin privileges
		- I installed it on a virtual machine and just copied the game folder to the real target machine
	2. The persistent hacks need to be run only once. Otherwise it will create redundant backups
	3. Needs more runtime hacks ;-)
*/

namespace R6BO
{
	// The game crashes when terrorist count is above 100
	constexpr DWORD NewTerroristMax = 100;
	const std::regex MissionRegex(R"((\".+\")(\s+)\d+)");

	void ApplyPersistentHacks(const std::filesystem::path& r6boHome)
	{
		std::filesystem::path maxTerroristTxtPath =
			r6boHome /
			L"mods\\Black Ops\\mission\\MaxTerrorists.txt";

		const std::filesystem::path backupPath = FsOps::BackupRename(maxTerroristTxtPath);

		std::ifstream input;
		input.exceptions(std::fstream::badbit);
		input.open(backupPath);

		std::ofstream output;
		output.exceptions(std::fstream::failbit | std::fstream::badbit);
		output.open(maxTerroristTxtPath);

		std::string line;
		std::smatch match;

		while (std::getline(input, line))
		{
			if (std::regex_search(line, match, MissionRegex))
			{
				output << match[1] << match[2] << NewTerroristMax << std::endl;
			}
			else
			{
				output << line << std::endl;
			}
		}

		Log << maxTerroristTxtPath << "stabbed";
		Log << "Backup @" << backupPath;

		std::wstring registryPath = 
			L"Software\\Classes\\VirtualStore\\MACHINE\\SOFTWARE\\Wow6432Node\\Red Storm Entertainment\\Black Ops";

		Registry userReg(HKEY_CURRENT_USER, registryPath);

		userReg.Write<DWORD>(L"CustomMissionNumberOfTerrorists", NewTerroristMax);
		userReg.Write<DWORD>(L"MaximumNumberOfTerrorists", NewTerroristMax);
		userReg.Write<DWORD>(L"MultiplayerNumberOfTerrorists", NewTerroristMax);
		userReg.Write<DWORD>(L"SelectedNumberOfTerrorists", NewTerroristMax);

		Log << "Registry entries stabbed";
	}
}

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CmdArgs args(givenArguments,
	{
		{ "path", typeid(std::filesystem::path), "Path to the installation directory" },
		{ "persistent", typeid(std::nullopt), "Hack the registry & global terrorist max limit file. The game does not need to be running." },
		{ "infammo", typeid(std::nullopt), "Infinite ammo. The game needs to be running." },
	});

	const std::filesystem::path r6boHome = args.Value<std::filesystem::path>("path");
	const std::filesystem::path exePath = r6boHome / L"R6BO.exe";

	if (SHA256(exePath) != "50d9413f4fca68205d2ff73a123e37898fb294274edbb4fb665b9e98a7a2a06e")
	{
		LogError << "Expected a clean installation of Rainbow Six: Black Ops 2.0 - Release (11/22 Update)";
		return ERROR_INVALID_IMAGE_HASH;
	}

	if (args.Contains("persistent"))
	{
		R6BO::ApplyPersistentHacks(r6boHome);
	}

	if (args.Contains("infammo"))
	{
		DWORD pid = System::WaitForWindow(L"R6BO.exe");

		Process process(pid);

		// Increasing ammo :-)
		process.ChangeByte(0x00215D1F, X86::SubGvEv, X86::AddGvEv);
	}

	return 0;
}
