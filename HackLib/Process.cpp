#include "HackLib-PCH.hpp"
#include "Process.hpp"

DWORD PidByName(std::wstring_view moduleName) {

	AutoHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));

	if (!snapshot)
	{
		throw Win32Exception("CreateToolhelp32Snapshot");
	}

	PROCESSENTRY32W processEntry = {};
	processEntry.dwSize = sizeof(PROCESSENTRY32W);

	if (!Process32FirstW(snapshot, &processEntry))
	{
		throw Win32Exception("Process32First");
	}

	do
	{
		if (moduleName.compare(processEntry.szExeFile) == 0)
		{
			return processEntry.th32ProcessID;
		}
	} while (Process32NextW(snapshot, &processEntry));

	throw RangeException("PID not found.");
}

MODULEENTRY32W ModuleByPid(DWORD pid) {

	AutoHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid));

	if (!snapshot)
	{
		throw Win32Exception("CreateToolhelp32Snapshot");
	}

	MODULEENTRY32W moduleEntry = {};
	moduleEntry.dwSize = sizeof(MODULEENTRY32W);

	if (!Module32FirstW(snapshot, &moduleEntry))
	{
		throw Win32Exception("Module32FirstW");
	}

	do
	{
		if (pid == moduleEntry.th32ProcessID)
		{
			return moduleEntry;
		}
	} while (Module32NextW(snapshot, &moduleEntry));

	throw RangeException("Module not found.");
}

Process::Process(DWORD pid) :
	_pid(pid),
	_module(ModuleByPid(pid)),
	_handle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid))
{
}

Process::Process(std::wstring_view name) :
	Process(PidByName(name))
{
}

BYTE* Process::BaseAddress() const
{
	return _module.modBaseAddr;
}
