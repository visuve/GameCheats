#pragma once

#include "Pointer.hpp"

class VirtualMemory
{
public:
	inline VirtualMemory(const Handle& parentProcess, size_t size) :
		_parentProcess(parentProcess),
		_address(VirtualAllocEx(parentProcess.Value(), nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
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
			_ASSERT(result);
			(void)result;
		}
	}

	inline MEMORY_BASIC_INFORMATION Query() const
	{
		constexpr DWORD memInfoSize = sizeof(MEMORY_BASIC_INFORMATION);
		MEMORY_BASIC_INFORMATION info = {};

		if (VirtualQueryEx(_parentProcess.Value(), _address, &info, memInfoSize) != memInfoSize)
		{
			throw Win32Exception("VirtualQueryEx");
		}

		return info;
	}

	inline Pointer Address() const
	{
		return _address;
	}

	bool operator < (const VirtualMemory& other) const
	{
		return _address < other._address;
	}

	bool operator == (const VirtualMemory& other) const
	{
		return _address == other._address;
	}

private:
	const Handle& _parentProcess;
	const Pointer _address;
};