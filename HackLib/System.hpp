#pragma once

namespace System
{
	DWORD PidByName(std::wstring_view moduleName);
	MODULEENTRY32W ModuleEntryByPid(DWORD pid);
	MODULEENTRY32W ModuleEntryByName(DWORD pid, std::wstring_view name);

	DWORD WaitForExe(std::wstring_view name);
	DWORD WaitForWindow(std::wstring_view name);

	size_t PageSize();

	// NOTE: probably not the most secure, 
	// but the Win32 functions were so horrible
	std::string GenerateGuid();

	std::filesystem::path CurrentExecutablePath();
	std::filesystem::path WindowsDirectory();
	std::filesystem::path SystemDirectory();

	void BeepUp();
	void BeepBurst();
	void BeepDown();
};

