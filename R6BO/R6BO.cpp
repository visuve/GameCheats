#include "R6BO-PCH.hpp"
#include "../HackLib/Registry.hpp"
#include "../HackLib/Process.hpp"
#include "../HackLib/OpCodes.hpp"

#include <filesystem>
#include <fstream>
#include <regex>
#include <functional>
#include <chrono>

/*
	More terrorists in Rainbow Six - Black Ops
	Tested with SHA-256
	06ee11a05a029a9827c093caa67de63c395c03082f7bc843ac302eadb9ff6373

	NOTES:
	1. Remember to build and run x86
	2. The non-persistent hacks have to be applied when the "Custom Mission" menu is open
		- The UI might not reflect the values immediately, but they are there
	3. The game appears to crash with terrorist count above 100
	4. The mission file needs to have enough spawnable terrorists between the BeginTeams & EndTeams blocks
		in order to work properly. This is on my todo list
*/


using PathFunction = std::function<void(const std::filesystem::path&)>;

void ProcessDirectory(const std::filesystem::path path, const std::span<PathFunction>& functions, bool recursive = true)
{
	for (const auto& iter : std::filesystem::recursive_directory_iterator(path))
	{
		for (const PathFunction& function : functions)
		{
			function(iter.path());
		}
	}
}

void BackupRename(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path))
	{
		return;
	}

	const auto now = std::chrono::system_clock::now();
	const auto sinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());

	std::wstring extension = std::format(L".{}.bak", sinceEpoch.count());
	auto backupPath = std::filesystem::path(path).replace_extension(extension);

	std::filesystem::rename(path, backupPath);
}

void ApplyHacks(bool persistent)
{
	if (persistent)
	{
		constexpr DWORD NewTerroristMax = 100;

		Registry reg(HKEY_CURRENT_USER, L"Software\\Classes\\VirtualStore\\MACHINE\\SOFTWARE\\Wow6432Node\\Red Storm Entertainment\\Black Ops");

		std::filesystem::path modsPath = reg.Read<std::wstring>(L"InstallationPath") + reg.Read<std::wstring>(L"ModsPath");
		auto maxTerroristTxtPath = reg.Read<std::filesystem::path>(L"MissionPath") / L"MaxTerrorists.txt";

		BackupRename(maxTerroristTxtPath);

		std::ofstream maxTerroristTxtFile;
		maxTerroristTxtFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		maxTerroristTxtFile.open(maxTerroristTxtPath);

		const auto terroristMaxFileOverwrite = [&maxTerroristTxtFile](const std::filesystem::path& path)
		{
			const std::wstring extension = path.extension();

			if (std::regex_search(extension, std::wregex(L"lwf|mis|tht", std::regex::icase)))
			{
				maxTerroristTxtFile << path.filename() << "\t\t" << NewTerroristMax << std::endl;
			}
		};

		const auto disableModSpecificMaxTerroristsOverride = [](const std::filesystem::path& path)
		{
			if (path.filename() == L"MaxTerrorists.txt")
			{
				BackupRename(path);
			}
		};

		PathFunction functions[] =
		{
			terroristMaxFileOverwrite,
			disableModSpecificMaxTerroristsOverride
		};

		ProcessDirectory(modsPath, functions, true);

		reg.Write<DWORD>(L"CustomMissionNumberOfTerrorists", NewTerroristMax);
		reg.Write<DWORD>(L"MaximumNumberOfTerrorists", NewTerroristMax);
		reg.Write<DWORD>(L"MultiplayerNumberOfTerrorists", NewTerroristMax);
		reg.Write<DWORD>(L"SelectedNumberOfTerrorists", NewTerroristMax);
	}
	else
	{
		Process process(L"R6BO.exe");

		// Skips any overrides with registry forced values
		BYTE* jnb = process.BaseAddress() + 0x0010A0AE;

		if (process.Read<BYTE>(jnb) == X86::JnbJb)
		{
			process.Write<BYTE>(jnb, X86::JbeJb);
		}

		// Forces selected & maximum values
		BYTE* base = process.BaseAddress() + 0x0046CDA4;

		BYTE* backgroundMax = process.FindPointer(base, { 0x10, 0x420 });
		BYTE* backgroundSelected = process.FindPointer(base, { 0x99C });
		BYTE* uiMax = process.FindPointer(base, { 0x8, 0xD4, 0x18C });
		BYTE* uiSelected = process.FindPointer(base, { 0x8, 0xD4, 0x184 });

		std::cout << process.Read<DWORD>(backgroundMax) << std::endl;
		std::cout << process.Read<DWORD>(backgroundSelected) << std::endl;
		std::cout << process.Read<DWORD>(uiMax) << std::endl;
		std::cout << process.Read<DWORD>(uiSelected) << std::endl;

		process.Write<DWORD>(backgroundMax, 100);
		process.Write<DWORD>(backgroundSelected, 100);
		process.Write<DWORD>(uiMax, 100);
		process.Write<DWORD>(uiSelected, 100);
	}
}

int wmain(int argc, wchar_t** argv)
{
	try
	{
		ApplyHacks(argc > 1 && std::wstring(argv[1]) == L"persistent");
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return -1;
}
