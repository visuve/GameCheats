#include "HackLib.hpp"

/*
	NOTES:
	1. The non-persistent hacks have to be applied when the "Custom Mission" menu is open
		- The UI might not reflect the values immediately, but they are there
	2. The persistent hacks create lots of backups of the mission files etc
*/

namespace R6BO
{
	// The game crashes when terrorist count is above 100
	constexpr DWORD NewTerroristMax = 100;
	const std::wregex MissionFileRegex(L"lwf|mis|tht", std::regex::icase);
	const std::regex MemberRegex(R"("Random(\d+)Team"(.+))");

	void TweakMissionFile(const std::filesystem::path& path)
	{
		std::smatch match;

		uint32_t highestId = 0;
		uint32_t highestLineNum = 0;
		std::string highestLine;

		// Find the highest ID and it's line in the file
		const auto predicate = [&](uint32_t lineNum, const std::string& line)
		{
			if (std::regex_search(line, match, MemberRegex))
			{
				uint32_t id = std::stoul(match[1]);

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

		if (FsOps::CountLines(path, predicate) != highestId)
		{
			throw LogicException("Terrorist count does not match with highest ID!");
		}

		if (highestId >= NewTerroristMax)
		{
			Log << path << " is already tweaked!" ;
			return;
		}

		const auto mutator = [&](uint32_t lineNum, const std::string& line, std::ostream& output)
		{
			output << line ;

			if (lineNum < highestLineNum)
			{
				return;
			}

			while (++highestId <= NewTerroristMax)
			{
				output << std::regex_replace(
					highestLine, MemberRegex, std::format("\"Random{}Team\"$2", highestId)) ;
			}
		};

		FsOps::Replicate(FsOps::BackupRename(path), mutator, path);
	}

	void ApplyPersistentHacks()
	{
		Registry reg(
			HKEY_CURRENT_USER,
			L"Software\\Classes\\VirtualStore\\MACHINE\\SOFTWARE\\Wow6432Node\\Red Storm Entertainment\\Black Ops");

		std::filesystem::path modsPath = reg.Read<std::wstring>(L"InstallationPath") + reg.Read<std::wstring>(L"ModsPath");
		auto maxTerroristTxtPath = reg.Read<std::filesystem::path>(L"MissionPath") / L"MaxTerrorists.txt";

		FsOps::BackupRename(maxTerroristTxtPath);

		std::ofstream maxTerroristTxtFile;
		maxTerroristTxtFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		maxTerroristTxtFile.open(maxTerroristTxtPath);

		const auto disableModSpecificMaxTerroristsOverride = [](const std::filesystem::path& path)
		{
			if (path.filename() == L"MaxTerrorists.txt")
			{
				FsOps::BackupRename(path);
				Log << "Disabled:" << path ;
			}

			return true;
		};

		const auto tweakMissionFiles = [&](const std::filesystem::path& path)
		{
			const std::wstring extension = path.extension();

			if (std::regex_search(extension, MissionFileRegex))
			{
				TweakMissionFile(path);

				maxTerroristTxtFile << path.filename() << "\t\t" << NewTerroristMax ;

				Log << "Tweaked:" << path ;
			}

			return true;
		};

		FsOps::PathFunction functions[] =
		{
			disableModSpecificMaxTerroristsOverride,
			tweakMissionFiles
		};

		FsOps::ProcessDirectory(modsPath, functions);

		reg.Write<DWORD>(L"CustomMissionNumberOfTerrorists", NewTerroristMax);
		reg.Write<DWORD>(L"MaximumNumberOfTerrorists", NewTerroristMax);
		reg.Write<DWORD>(L"MultiplayerNumberOfTerrorists", NewTerroristMax);
		reg.Write<DWORD>(L"SelectedNumberOfTerrorists", NewTerroristMax);
	}

	void HackRunningProcess()
	{
		DWORD pid = System::WaitForExe(L"R6BO.exe");

		Process process(pid);

		if (!process.Verify("06ee11a05a029a9827c093caa67de63c395c03082f7bc843ac302eadb9ff6373"))
		{
			LogError << "Expected Rainbow Six - Black Ops";
			return;
		}

		// Skips any overrides with registry forced values
		process.ChangeByte(0x0010A0AE, X86::JnbJb, X86::JbeJb);

		// Increasing ammo :-)
		process.ChangeByte(0x00215D1F, X86::SubGvEv, X86::AddGvEv);

		// Forces selected & maximum values
		constexpr size_t base = 0x0046CDA4;

		Pointer backgroundMax = process.ResolvePointer(base, 0x10u, 0x420u);
		Pointer backgroundSelected = process.ResolvePointer(base, 0x99Cu);
		Pointer uiMax = process.ResolvePointer(base, 0x8u, 0xD4u, 0x18Cu);
		Pointer uiSelected = process.ResolvePointer(base, 0x8u, 0xD4u, 0x184u);

		Log << process.Read<uint32_t>(backgroundMax) ;
		Log << process.Read<uint32_t>(backgroundSelected) ;
		Log << process.Read<uint32_t>(uiMax) ;
		Log << process.Read<uint32_t>(uiSelected) ;

		process.Write<uint32_t>(backgroundMax, 100);
		process.Write<uint32_t>(backgroundSelected, 100);
		process.Write<uint32_t>(uiMax, 100);
		process.Write<uint32_t>(uiSelected, 100);
	}
}

int wmain(int argc, wchar_t** argv)
{
	try
	{
		const CmdArgs args(argc, argv,
		{
			{ L"persistent", typeid(std::nullopt), L"Hack the registry & some mission files to allow more terrorists" },
			{ L"persistent", typeid(std::nullopt), L"Apply various in memory hacks, e.g. increasing ammo" },
		});

		if (args.Contains(L"persistent"))
		{
			R6BO::ApplyPersistentHacks();
		}

		if (args.Contains(L"inmemory"))
		{
			R6BO::HackRunningProcess();
		}
	}
	catch (const CmdArgs::Exception& e)
	{
		LogError << '\n' << e.what() << "!\n";
		std::wcerr << e.Usage() ;
		return ERROR_BAD_ARGUMENTS;
	}
	catch (const std::system_error& e)
	{
		LogError << e.what();
		return e.code().value();
	}
	catch (const std::exception& e)
	{
		LogError << e.what() ;
		return ERROR_PROCESS_ABORTED;
	}

	return 0;
}
