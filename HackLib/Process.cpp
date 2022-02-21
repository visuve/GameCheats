#include "HackLib-PCH.hpp"
#include "Process.hpp"
#include "OpCodes.hpp"

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
		const std::function<bool(const T&)>& filter) const
	{
		T entry = {};
		entry.dwSize = sizeof(T);

		if (!first(_handle, &entry))
		{
			throw Win32Exception("Failed to iterate");
		}

		do
		{
			if (filter(entry))
			{
				return entry;
			}

		} while (next(_handle, &entry));

		throw RangeException("Item not found.");
	}

	PROCESSENTRY32W FindProcess(const std::function<bool(const PROCESSENTRY32W&)>& filter) const
	{
		return Find<PROCESSENTRY32W>(&Process32FirstW, &Process32NextW, filter);
	}

	MODULEENTRY32W FindModule(const std::function<bool(const MODULEENTRY32W&)>& filter) const
	{
		return Find<MODULEENTRY32W>(&Module32FirstW, &Module32NextW, filter);
	}

private:
	HANDLE _handle = nullptr;
};

DWORD PidByName(std::wstring_view moduleName) {

	const Snapshot snapshot(TH32CS_SNAPPROCESS, 0);

	const auto filter = [&](const PROCESSENTRY32W& processEntry)
	{
		return moduleName.compare(processEntry.szExeFile) == 0;
	};

	return snapshot.FindProcess(filter).th32ProcessID;
}

MODULEENTRY32W ModuleByPid(DWORD pid) {

	const Snapshot snapshot(TH32CS_SNAPMODULE, pid);

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

Pointer Process::AllocateMemory(size_t size)
{
	void* memory = VirtualAllocEx(_handle, nullptr, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!memory)
	{
		throw Win32Exception("VirtualAllocEx");
	}

	auto result = _memory.emplace(static_cast<uint8_t*>(memory));

	_ASSERT_EXPR(result.second, L"Catastrophic failure, pointer already existed!");

	return *result.first;
}

void Process::FreeMemory(Pointer pointer)
{
	const auto it = std::find(_memory.cbegin(), _memory.cend(), pointer);

	if (it == _memory.cend())
	{
		throw RangeException("No such memory allocated!");
	}

	_memory.erase(it);

	if (!VirtualFreeEx(_handle, pointer, 0, MEM_RELEASE))
	{
		throw Win32Exception("VirtualFreeEx");
	}
}