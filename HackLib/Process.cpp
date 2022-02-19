#include "HackLib-PCH.hpp"
#include "Process.hpp"

class Snapshot
{
public:
	Snapshot(DWORD flags, DWORD pid) :
		_handle(CreateToolhelp32Snapshot(flags, pid))
	{
		if (!_handle)
		{
			throw Win32Exception("CreateToolhelp32Snapshot");
		}
	}

	NonCopyable(Snapshot);

	virtual ~Snapshot()
	{
		if (_handle)
		{
			CloseHandle(_handle);
		}
	}

	template <typename T>
	T Find(
		BOOL (WINAPI *first)(HANDLE, T*),
		BOOL(WINAPI *next)(HANDLE, T*),
		const std::function<bool(const T&)>& filter)
	{
		T processEntry = {};
		processEntry.dwSize = sizeof(T);

		if (!first(_handle, &processEntry))
		{
			throw Win32Exception("Failed to iterate");
		}

		do
		{
			if (filter(processEntry))
			{
				return processEntry;
			}

		} while (next(_handle, &processEntry));

		throw RangeException("Item not found.");
	}

	PROCESSENTRY32W FindProcess(const std::function<bool(const PROCESSENTRY32W&)>& filter)
	{
		return Find<PROCESSENTRY32W>(&Process32FirstW, &Process32NextW, filter);
	}

	MODULEENTRY32W FindModule(const std::function<bool(const MODULEENTRY32W&)>& filter)
	{
		return Find<MODULEENTRY32W>(&Module32FirstW, &Module32NextW, filter);
	}

private:
	HANDLE _handle = nullptr;
};

DWORD PidByName(std::wstring_view moduleName) {

	Snapshot snapshot(TH32CS_SNAPPROCESS, 0);

	const auto filter = [&](const PROCESSENTRY32W& processEntry)
	{
		return moduleName.compare(processEntry.szExeFile) == 0;
	};

	return snapshot.FindProcess(filter).th32ProcessID;
}

MODULEENTRY32W ModuleByPid(DWORD pid) {

	Snapshot snapshot(TH32CS_SNAPMODULE, pid);

	const auto filter = [&](const MODULEENTRY32W& moduleEntry)
	{
		return pid == moduleEntry.th32ProcessID;
	};

	return snapshot.FindModule(filter);
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

Process::~Process()
{
	if (_handle)
	{
		CloseHandle(_handle);
	}
}

uint8_t* Process::Address(size_t offset) const
{
	return _module.modBaseAddr + offset;
}
