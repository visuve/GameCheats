#pragma once

#include "Win32Process.hpp"

class VirtualMemory
{
public:
	inline VirtualMemory(const Win32Process& parentProcess, size_t size) :
		_parentProcess(parentProcess),
		_address(VirtualAllocEx(
			_parentProcess.Value(),
			nullptr,
			size,
			MEM_COMMIT | MEM_RESERVE,
			PAGE_EXECUTE_READWRITE))
	{
		if (!_address)
		{
			throw Win32Exception("VirtualAllocEx");
		}
	}

	inline ~VirtualMemory()
	{
		if (_parentProcess.IsValid())
		{
			bool result = VirtualFreeEx(_parentProcess.Value(), _address, 0, MEM_RELEASE);
			_ASSERT_EXPR(result, "Could not free virtual memory");
			(void)result;
		}
	}

	inline Pointer Address() const
	{
		return _address;
	}

	inline MEMORY_BASIC_INFORMATION Query() const
	{
		if (!_parentProcess.IsValid())
		{
			throw RuntimeException("The parent process is invalid");
		}

		constexpr DWORD memInfoSize = sizeof(MEMORY_BASIC_INFORMATION);
		MEMORY_BASIC_INFORMATION info = {};

		if (VirtualQueryEx(_parentProcess.Value(), _address, &info, memInfoSize) != memInfoSize)
		{
			throw Win32Exception("VirtualQueryEx");
		}

		return info;
	}

	inline bool operator < (const VirtualMemory& other) const
	{
		return _address < other._address;
	}

	inline bool operator == (const VirtualMemory& other) const
	{
		return _address == other._address;
	}

private:
	const Win32Process& _parentProcess;
	const Pointer _address;
};