#pragma once

#include "Win32Handle.hpp"
#include "NonCopyable.hpp"

class Win32Thread : public Win32Handle
{
public:
	Win32Thread(HANDLE handle);
	Win32Thread(DWORD identifier, DWORD access = THREAD_ALL_ACCESS);
	Win32Thread(std::function<uint32_t(std::stop_token)> callback);
	~Win32Thread();

	NonCopyable(Win32Thread);

	void Wait(std::chrono::milliseconds timeout = std::chrono::milliseconds(INFINITE)) const;

	DWORD ExitCode();
	DWORD Identifier();

private:
	static DWORD ThreadProcedure(void*);

	DWORD _identifier = 0;
	std::stop_source _stopSource;
	std::function<uint32_t()> _callback;
};