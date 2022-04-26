#include "Exceptions.hpp"
#include "Handle.hpp"
#include "Logger.hpp"
#include "NonCopyable.hpp"
#include "StrConvert.hpp"
#include "System.hpp"

#define UNUSED(x) (void)x;

BOOL WINAPI ConsoleHandler(DWORD signal)
{
	Log << "Signaled" << signal;
	return SetEvent(System::Instance().WaitEvent);
}

class Snapshot : public Handle
{
public:
	Snapshot(DWORD flags, DWORD pid) :
		Handle(CreateToolhelp32Snapshot(flags, pid))
	{
		if (!IsValid())
		{
			throw Win32Exception("CreateToolhelp32Snapshot");
		}
	}

	NonCopyable(Snapshot);

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

	_ASSERT(title.size() == static_cast<size_t>(length));

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

	GetSystemInfo(&_systemInfo);
}

System::~System()
{
	if (SetEvent(WaitEvent))
	{
		bool result = CloseHandle(WaitEvent);
		_ASSERT(result);
		UNUSED(result);
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
			std::format("Process \"{}\" not found", StrConvert::ToUtf8(moduleName)));
	}

	return result.value().th32ProcessID;
}

MODULEENTRY32W System::ModuleEntryByPid(DWORD pid)
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

MODULEENTRY32W System::ModuleEntryByName(DWORD pid, std::wstring_view name)
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
			std::format("Module \"{}\" not found", StrConvert::ToUtf8(name)));
	}

	return result.value();
}


DWORD System::WaitForExe(std::wstring_view name)
{
	const auto filter = [&](const PROCESSENTRY32W& processEntry)
	{
		return name.compare(processEntry.szExeFile) == 0;
	};

	DWORD waitResult = WAIT_FAILED;

	do
	{
		const Snapshot snapshot(TH32CS_SNAPPROCESS, 0);

		auto result = snapshot.FindProcess(filter);

		if (result.has_value())
		{
			return result.value().th32ProcessID;
		}

		Log << "Process" << Logger::Quoted << StrConvert::ToUtf8(name) << "has not appeared yet..." ;

		waitResult = WaitForSingleObject(WaitEvent, 2000);

	} while (waitResult == WAIT_TIMEOUT);

	if (waitResult == WAIT_OBJECT_0 && !ResetEvent(WaitEvent))
	{
		throw Win32Exception("ResetEvent");
	}

	throw RuntimeException("Aborted");
}

DWORD System::WaitForWindow(std::wstring_view name)
{
	DWORD waitResult = WAIT_FAILED;

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

		Log << "Window" << Logger::Quoted << StrConvert::ToUtf8(name) << "has not appeared yet..." ;

		waitResult = WaitForSingleObject(WaitEvent, 2000);

	} while (waitResult == WAIT_TIMEOUT);

	if (waitResult == WAIT_OBJECT_0 && !ResetEvent(WaitEvent))
	{
		throw Win32Exception("ResetEvent");
	}

	throw RuntimeException("Aborted");
}

size_t System::PageSize()
{
	return System::Instance()._systemInfo.dwPageSize;
}

std::string System::GenerateGuid()
{
	union Hack
	{
		GUID Guid; // See guiddef.h
		uint64_t Data[2];
	} hack = {};

	thread_local std::random_device device;
	thread_local std::mt19937 engine(device());
	thread_local std::uniform_int_distribution<uint64_t> distribution(
		std::numeric_limits<uint64_t>::min(),
		std::numeric_limits<uint64_t>::max());

	hack.Data[0] = distribution(engine);
	hack.Data[1] = distribution(engine);

	return std::format(
		"{{{:08X}-{:04X}-{:04X}-{:02X}{:02X}-{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}}}",
		hack.Guid.Data1,
		hack.Guid.Data2,
		hack.Guid.Data3,
		hack.Guid.Data4[0],
		hack.Guid.Data4[1],
		hack.Guid.Data4[2],
		hack.Guid.Data4[3],
		hack.Guid.Data4[4],
		hack.Guid.Data4[5],
		hack.Guid.Data4[6],
		hack.Guid.Data4[7]);
}