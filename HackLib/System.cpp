#include "System.hpp"
#include "../Mega.pch"

BOOL WINAPI ConsoleHandler(DWORD signal)
{
	std::cout << "Signaled " << signal << std::endl;
	return SetEvent(System::Instance().WaitEvent);
}

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
	std::optional<T> Find(
		BOOL(WINAPI* first)(HANDLE, T*),
		BOOL(WINAPI* next)(HANDLE, T*),
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

		return {};
	}

	std::optional<PROCESSENTRY32W> FindProcess(const std::function<bool(const PROCESSENTRY32W&)>& filter) const
	{
		return Find<PROCESSENTRY32W>(&Process32FirstW, &Process32NextW, filter);
	}

	std::optional<MODULEENTRY32W> FindModule(const std::function<bool(const MODULEENTRY32W&)>& filter) const
	{
		return Find<MODULEENTRY32W>(&Module32FirstW, &Module32NextW, filter);
	}

private:
	HANDLE _handle = nullptr;
};

std::optional<std::wstring> WindowTitle(HWND window) 
{
	if (!IsWindowVisible(window))
	{
		return {};
	}

	int length = GetWindowTextLengthW(window);

	if (length <= 0)
	{
		return {};
	}

	std::wstring title(length, '\0');

	length = GetWindowTextW(window, title.data(), static_cast<int>(title.size() + 1));

	_ASSERT(length == title.size());

	return title;
}

System::System()
{
	WaitEvent = CreateEventW(nullptr, true, false, L"Exit");

	if (!WaitEvent)
	{
		throw Win32Exception("CreateEventW");
	}

	if (!SetConsoleCtrlHandler(ConsoleHandler, true))
	{
		throw Win32Exception("SetConsoleCtrlHandler");
	}
}

System::~System()
{
	if (SetEvent(WaitEvent))
	{
		bool result = CloseHandle(WaitEvent);
		_ASSERT(result);
	}
}

System& System::Instance()
{
	static System instance;
	return instance;
}

DWORD System::PidByName(std::wstring_view moduleName)
{
	const Snapshot snapshot(TH32CS_SNAPPROCESS, 0);

	const auto filter = [&](const PROCESSENTRY32W& processEntry)
	{
		return moduleName.compare(processEntry.szExeFile) == 0;
	};

	auto result = snapshot.FindProcess(filter);

	if (!result.has_value())
	{
		throw RangeException(
			std::format("Process {} not found", StrConvert::ToUtf8(moduleName)));
	}

	return result.value().th32ProcessID;
}

MODULEENTRY32W System::ModuleByPid(DWORD pid)
{
	if (pid == 0)
	{
		throw Win32Exception("Cannot handle PID 0");
	}

	const Snapshot snapshot(TH32CS_SNAPMODULE, pid);

	const auto filter = [&](const MODULEENTRY32W& moduleEntry)
	{
		return pid == moduleEntry.th32ProcessID;
	};

	auto result = snapshot.FindModule(filter);

	if (!result.has_value())
	{
		throw RangeException("Module not found");
	}

	return result.value();
}

MODULEENTRY32W System::ModuleByName(DWORD pid, std::wstring_view name)
{
	const Snapshot snapshot(TH32CS_SNAPMODULE, pid);

	const auto filter = [&](const MODULEENTRY32W& moduleEntry)
	{
		const std::filesystem::path path(moduleEntry.szModule);
		return _wcsnicmp(path.c_str(), name.data(), name.size()) == 0;
	};

	auto result = snapshot.FindModule(filter);

	if (!result.has_value())
	{
		throw RangeException(
			std::format("Module {} not found", StrConvert::ToUtf8(name)));
	}

	return result.value();
}

DWORD System::WaitForWindow(std::wstring_view name)
{
	do
	{
		for (HWND window = GetTopWindow(nullptr); window; window = GetWindow(window, GW_HWNDNEXT))
		{
			std::optional<std::wstring> title = WindowTitle(window);

			if (!title.has_value() || title != name)
			{
				continue;
			}

			DWORD pid = 0;

			if (!GetWindowThreadProcessId(window, &pid))
			{
				throw Win32Exception("GetWindowThreadProcessId");
			}

			return pid;
		}

		std::cout << "Window \"" << StrConvert::ToUtf8(name) << "\" has not appeared yet..." << std::endl;

	} while (WaitForSingleObject(WaitEvent, 2000) == WAIT_TIMEOUT);

	throw RuntimeException("Aborted");
}