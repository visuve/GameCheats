#pragma once

#include "NonCopyable.hpp"
#include "Pointer.hpp"
#include "Win32Handle.hpp"

class Win32Process : public Win32Handle
{
public:
	explicit Win32Process(DWORD desiredAccess, DWORD pid);

	NonCopyable(Win32Process);

	HANDLE Value() const;

	std::filesystem::path Path() const;

	DWORD WaitForInputIdle(std::chrono::milliseconds timeout) const;

	DWORD WaitForExit(std::chrono::milliseconds timeout) const;

	size_t ReadProcessMemory(Pointer pointer, void* buffer, size_t size) const;

	size_t WriteProcessMemory(Pointer pointer, const void* value, size_t size) const;

	MEMORY_BASIC_INFORMATION VirtualQueryEx(Pointer pointer) const;

	DWORD VirtualProtectEx(Pointer pointer, size_t size, DWORD newAccess) const;

	HANDLE CreateRemoteThread(Pointer startAddress, Pointer parameter) const;

	void FlushInstructionCache(Pointer address, size_t size) const;

	DWORD ExitCode();

};