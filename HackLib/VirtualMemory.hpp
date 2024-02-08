#pragma once

#include "Win32Process.hpp"
#include "NonCopyable.hpp"

class VirtualMemory
{
public:
	VirtualMemory(const Win32Process& parentProcess, size_t size);
	~VirtualMemory();
	NonCopyable(VirtualMemory);

	Pointer Address() const;
	bool operator < (const VirtualMemory& other) const;
	bool operator == (const VirtualMemory& other) const;

	MEMORY_BASIC_INFORMATION Query() const;

private:
	const Win32Process& _parentProcess;
	const Pointer _address;
};