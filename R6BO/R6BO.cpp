#include "R6BO-PCH.hpp"
#include "../HackLib/Registry.hpp"
#include "../HackLib/Process.hpp"
#include "../HackLib/OpCodes.hpp"

#include <filesystem>
#include <fstream>
#include <regex>

/*
	More terrorists in Rainbow Six - Black Ops
	Tested with SHA-256
	06ee11a05a029a9827c093caa67de63c395c03082f7bc843ac302eadb9ff6373

	NOTES:
	1. Remember to build and run x86, the registry redirection will go wrong otherwise!
	2. The non-persistent hacks have to be applied when the "Custom Mission" menu is open
		- The UI might not reflect the values immediately, but they are there
	3. The game appears to crash with terrorist count above 100
	4. The mission file needs to have enough spawnable terrorists between the BeginTeams & EndTeams blocks
		in order to work properly. This is on my todo list
*/

void SetTerroristMaxRegistry(const Registry& reg, DWORD value)
{
	const auto keys =
	{
		L"CustomMissionNumberOfTerrorists",
		L"MaximumNumberOfTerrorists",
		L"MultiplayerNumberOfTerrorists",
		L"SelectedNumberOfTerrorists"
	};

	for (std::wstring_view key : keys)
	{
		if (reg.Read<DWORD>(key) != value)
		{
			reg.Write(key, value);
		}
	}
}

void SetTerroristMaxTxtOverride(const Registry& reg, DWORD value)
{
	std::filesystem::path modsPath =
		reg.Read<std::wstring>(L"InstallationPath") + reg.Read<std::wstring>(L"ModsPath");

	std::filesystem::path missionPath =
		reg.Read<std::wstring>(L"MissionPath");

	auto maxTerroristsGlobalOverridePath = missionPath / L"MaxTerrorists.txt";
	auto backup = missionPath / L"MaxTerrorists.bak";
	
	if (!std::filesystem::exists(backup))
	{
		std::filesystem::rename(maxTerroristsGlobalOverridePath, backup);
	}

	std::ofstream output;
	output.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	output.open(maxTerroristsGlobalOverridePath);

	for (const auto& iter : std::filesystem::recursive_directory_iterator(modsPath))
	{
		auto path = iter.path();
		auto filename = path.filename();
		auto extension = filename.extension().string();

		if (std::regex_search(extension, std::regex("lwf|mis|tht", std::regex::icase)))
		{
			output << filename << "\t\t" << value << std::endl;
		}
		
		// We do not want mod specific overrides
		if (filename == L"MaxTerrorists.txt")
		{
			backup = path;
			backup.replace_extension(L".bak");
			std::filesystem::rename(path, backup);
		}
	}
}

void ApplyHacks(bool persistent)
{
	if (persistent)
	{
		Registry registry(
			HKEY_CURRENT_USER,
			L"Software\\Classes\\VirtualStore\\MACHINE\\SOFTWARE\\Wow6432Node\\Red Storm Entertainment\\Black Ops");

		SetTerroristMaxRegistry(registry, 100);
		SetTerroristMaxTxtOverride(registry, 100);
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
