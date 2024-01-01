#include "HackLib.hpp"

/*
	NOTES:

	I would not install the game using the installer which is not signed and prompts for admin privileges
		- I installed it on a virtual machine and just copied the game folder to the real target machine
*/


namespace R6BO
{
	// The game crashes when terrorist count is above 100
	constexpr DWORD NewTerroristMax = 100;
	const std::regex MaxTerroristRegex(R"((\".+\")(\s+)(\d+))");
	const std::wregex MissionFileExtensionRegex(L"lwf|mis|tht", std::regex::icase);
	const std::regex MissionFileMemberRegex(R"("Random(\d+)Team"(.+))");

	// This function basically duplicates the last "member" aka terrorist
	// until the total number of "members" reaches 100
	void PatchMaxTerroristFile(const std::filesystem::path& path)
	{
		std::smatch match;

		const auto predicate = [&](uint32_t, const std::string& line)
		{
			if (std::regex_search(line, match, MaxTerroristRegex))
			{
				return match[3] != std::to_string(NewTerroristMax);
			}

			return false;
		};

		if (!FsOps::CountLines(path, predicate))
		{
			LogInfo << path << "appears already patched!";
			return;
		}

		const auto mutator = [&](uint32_t, const std::string& line, std::ostream& output)
		{
			if (std::regex_search(line, match, MaxTerroristRegex))
			{
				output << match[1] << match[2] << NewTerroristMax << std::endl;
			}
			else
			{
				output << line << std::endl;
			}
		};

		auto backup = FsOps::BackupRename(path);

		FsOps::Replicate(backup, mutator, path);

		LogInfo << "Patched:" << path;
		LogInfo << "Backup @" << backup;
	}

	void PatchMissionFile(const std::filesystem::path& path)
	{
		std::smatch match;

		uint32_t highestId = 0;
		uint32_t highestLineNum = 0;
		std::string highestLine;

		// Find the highest ID and it's line in the file
		const auto predicate = [&](uint32_t lineNum, const std::string& line)
		{
			if (std::regex_search(line, match, MissionFileMemberRegex))
			{
				uint32_t id = std::stoul(match[1]);

				if (id > highestId + 1)
				{
					LogWarning << "There is probably a typo in" << path << "@ line" << lineNum;
				}

				if (id > highestId)
				{
					highestId = id;
					highestLineNum = lineNum;
					highestLine = line;
				}

				return true;
			}

			return false;
		};

		uint32_t members = FsOps::CountLines(path, predicate);

		if (!members)
		{
			LogInfo << "No terrorists in" << path;
			return;
		}

		if (members != highestId)
		{
			LogError << "Terrorist count does not match with highest ID parsed @" << path;
			return;
		}

		if (highestId >= NewTerroristMax)
		{
			LogInfo << path << "appears already patched!";
			return;
		}

		const auto mutator = [&](uint32_t lineNum, const std::string& line, std::ostream& output)
		{
			output << line << std::endl;

			if (lineNum < highestLineNum)
			{
				return;
			}

			while (++highestId <= NewTerroristMax)
			{
				std::string newMemberId = std::format("\"Random{}Team\"$2", highestId);
				output << std::regex_replace(highestLine, MissionFileMemberRegex, newMemberId);
				output << std::endl;
			}
		};

		auto backup = FsOps::BackupRename(path);

		FsOps::Replicate(backup, mutator, path);

		LogInfo << "Patched:" << path;
		LogInfo << "Backup @" << backup;
	}

	void ApplyFileHacks(const std::filesystem::path& r6boHome)
	{
		const std::filesystem::path modsPath = r6boHome / L"mods";

		const auto patchMaxTerroristFiles = [](const std::filesystem::path& path)
		{
			if (path.filename() == L"MaxTerrorists.txt")
			{
				PatchMaxTerroristFile(path);
			}

			return true;
		};

		const auto patchMissionFiles = [&](const std::filesystem::path& path)
		{
			const std::wstring extension = path.extension();

			if (std::regex_search(extension, MissionFileExtensionRegex))
			{
				PatchMissionFile(path);
			}

			return true;
		};

		FsOps::PathFunction functions[] =
		{
			patchMaxTerroristFiles,
			patchMissionFiles
		};

		FsOps::ProcessDirectory(modsPath, functions);
	}

	void ApplyRegistryHacks()
	{
		std::wstring registryPath = 
			L"Software\\Classes\\VirtualStore\\MACHINE\\SOFTWARE\\Wow6432Node\\Red Storm Entertainment\\Black Ops";

		Registry userReg(HKEY_CURRENT_USER, registryPath);

		userReg.Write<DWORD>(L"CustomMissionNumberOfTerrorists", NewTerroristMax);
		userReg.Write<DWORD>(L"MaximumNumberOfTerrorists", NewTerroristMax);
		userReg.Write<DWORD>(L"MultiplayerNumberOfTerrorists", NewTerroristMax);
		userReg.Write<DWORD>(L"SelectedNumberOfTerrorists", NewTerroristMax);

		LogInfo << "Registry entries stabbed";
	}
}

int IWillNotUseHackLibForEvil(const std::vector<std::string>& givenArguments)
{
	const CommandLine args(givenArguments,
	{
		{ "path", typeid(std::filesystem::path), "Path to the installation directory. Needed for option \"persistent\"." },
		{ "persistent", typeid(std::nullopt), "Overrides maximum terrorists related values in mission files"
			", max terrorist limit files and Windows registry. The game does not need to be running." },
		{ "infammo", typeid(std::nullopt), "Infinite ammo. The game needs to be running." },
		{ "recoil", typeid(float), "Set recoil base. 1 is default, 0 is none. Affects all recoil calculation. Use zoom to enable." },
	});

	if (args.Contains("persistent"))
	{
		const std::filesystem::path r6boHome = args.Value<std::filesystem::path>("path");
		const std::filesystem::path exePath = r6boHome / L"R6BO.exe";

		if (SHA256(exePath) != "50d9413f4fca68205d2ff73a123e37898fb294274edbb4fb665b9e98a7a2a06e")
		{
			LogError << "Expected a clean installation of Rainbow Six: Black Ops 2.0 - Release (11/22 Update)";
			return ERROR_INVALID_IMAGE_HASH;
		}

		R6BO::ApplyFileHacks(r6boHome);
		R6BO::ApplyRegistryHacks();
		return 0;
	}

	DWORD pid = System::WaitForWindow(L"Black Ops");

	Process process(pid);

	if (args.Contains("infammo"))
	{
		// Increasing ammo :-)
		process.ChangeByte(0x00215D1F, X86::SubGvEv, X86::AddGvEv);
	}

	if (args.Contains("recoil"))
	{
		float recoil = args.Value<float>("recoil");
		process.Write(0x1D7416, recoil);
	}

	return 0;
}
