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

#ifdef _WIN64
void Process::InjectX64(size_t from, std::span<uint8_t> code)
{
	const size_t codeSize = code.size_bytes();
	const size_t bytesRequired = codeSize + X64::JumpOpSize;
	const size_t nops = codeSize - X64::JumpOpSize;

	_ASSERTE(nops < codeSize);

	Pointer origin = Address(from);
	Pointer target = AllocateMemory(bytesRequired);

	{
		// Add jump op size, because we dont want a forever loop
		Pointer backwards(origin + X64::JumpOpSize);

		std::vector<uint8_t> codeWithJumpBack(code.begin(), code.end());
		auto jump = X64::JumpAbsolute(backwards);
		std::copy(jump.cbegin(), jump.cend(), std::back_inserter(codeWithJumpBack));

		WriteBytes(target, codeWithJumpBack);

		if (!FlushInstructionCache(_handle, target, bytesRequired))
		{
			throw Win32Exception("FlushInstructionCache");
		}
	}

	{
		const auto jump = X64::JumpAbsolute(target);
		std::vector<uint8_t> detour(jump.cbegin(), jump.cend());
		std::fill_n(std::back_inserter(detour), nops, X86::Nop);

		WriteBytes(origin, detour);

		if (!FlushInstructionCache(_handle, origin, detour.size()))
		{
			throw Win32Exception("FlushInstructionCache");
		}
	}
}
#else
void Process::InjectX86(size_t from, std::span<uint8_t> code)
{
	const size_t codeSize = code.size_bytes();
	const size_t bytesRequired = codeSize + X86::JumpOpSize;
	const size_t nops = codeSize - X86::JumpOpSize;

	_ASSERTE(nops < codeSize);

	Pointer origin = Address(from);
	Pointer target = AllocateMemory(bytesRequired);

	{
		// Add jump op size, because we dont want a forever loop
		Pointer backwards((origin + X86::JumpOpSize) - (target + bytesRequired));
		
		std::vector<uint8_t> codeWithJumpBack(code.begin(), code.end());
		auto jump = X86::JumpRelative(backwards);
		std::copy(jump.cbegin(), jump.cend(), std::back_inserter(codeWithJumpBack));

		WriteBytes(target, codeWithJumpBack);

		if (!FlushInstructionCache(_handle, target, codeWithJumpBack.size()))
		{
			throw Win32Exception("FlushInstructionCache");
		}
	}

	{
		Pointer forwards(target - (origin + X86::JumpOpSize));
		
		const auto jump = X86::JumpRelative(forwards);
		std::vector<uint8_t> detour(jump.cbegin(), jump.cend());
		std::fill_n(std::back_inserter(detour), nops, X86::Nop);

		WriteBytes(origin, detour);

		if (!FlushInstructionCache(_handle, origin, detour.size()))
		{
			throw Win32Exception("FlushInstructionCache");
		}
	}
}
#endif