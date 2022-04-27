#pragma once

#include "Win32Process.hpp"

class VirtualMemory
{
public:
	inline VirtualMemory(const Win32Process& parentProcess, size_t size) :
		_parentProcess(parentProcess),
		_address(parentProcess.AllocateVirtualMemory(size))
	{
	}

	inline ~VirtualMemory()
	{
		if (_parentProcess.IsValid() && _address.Value())
		{
			bool result = _parentProcess.FreeVirtualMemory(_address);
			_ASSERT(result);
			(void)result;
		}
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
	const Win32Process& _parentProcess;
	const Pointer _address;
};