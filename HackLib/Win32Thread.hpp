#pragma once

#include "Win32Handle.hpp"
#include "Win32Event.hpp"
#include "NonCopyable.hpp"

class Win32Thread : public Win32Handle
{
public:
	Win32Thread(HANDLE handle);
	Win32Thread(DWORD identifier, DWORD access = THREAD_ALL_ACCESS);
	Win32Thread(std::function<uint32_t(const Win32Event&)> callback);
	~Win32Thread();

	NonCopyable(Win32Thread);

	DWORD Wait(std::chrono::milliseconds timeout = std::chrono::milliseconds(INFINITE)) const;

	DWORD ExitCode();
	DWORD Identifier();

private:
	static DWORD WINAPI ThreadProcedure(void*);

	DWORD _identifier = 0;
	Win32Event _event;
	std::function<uint32_t()> _callback;
};