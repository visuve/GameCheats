#include "VirtualMemory.hpp"
#include "VirtualMemory.hpp"
#include "VirtualMemory.hpp"

VirtualMemory::VirtualMemory(const Win32Process& parentProcess, size_t size) :
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

VirtualMemory::~VirtualMemory()
{
	if (_address && _parentProcess.IsValid())
	{
		[[maybe_unused]]
		bool result = VirtualFreeEx(_parentProcess.Value(), _address, 0, MEM_RELEASE);
		_ASSERT_EXPR(result, L"Could not free virtual memory");
	}
}

Pointer VirtualMemory::Address() const
{
	return _address;
}

bool VirtualMemory::operator < (const VirtualMemory& other) const
{
	return _address < other._address;
}

bool VirtualMemory::operator == (const VirtualMemory& other) const
{
	return _address == other._address;
}

MEMORY_BASIC_INFORMATION VirtualMemory::Query() const
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