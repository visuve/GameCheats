#pragma once

class System
{
public:
	System();
	~System();

	static System& Instance();

	static DWORD PidByName(std::wstring_view moduleName);
	static MODULEENTRY32W ModuleByPid(DWORD pid);
	static MODULEENTRY32W ModuleByName(DWORD pid, std::wstring_view name);

	DWORD WaitForWindow(std::wstring_view name);

	HANDLE WaitEvent = nullptr;
};

