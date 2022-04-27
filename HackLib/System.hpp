#pragma once

#include "NonCopyable.hpp"
#include "Win32Event.hpp"

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

	void BeepUp();
	void BeepBurst();
	void BeepDown();
};

