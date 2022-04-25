#pragma once

class System
{
public:
	System();
	~System();

	static System& Instance();

	static DWORD PidByName(std::wstring_view moduleName);
	static MODULEENTRY32W ModuleEntryByPid(DWORD pid);
	static MODULEENTRY32W ModuleEntryByName(DWORD pid, std::wstring_view name);

	DWORD WaitForExe(std::wstring_view name);
	DWORD WaitForWindow(std::wstring_view name);

	static size_t PageSize();

	// NOTE: probably not the most secure, 
	// but the Win32 functions were so horrible
	static std::string GenerateGuid();

	HANDLE WaitEvent = nullptr;

private:
	SYSTEM_INFO _systemInfo = {};
};

