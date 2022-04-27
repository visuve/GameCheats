#pragma once

#include "Win32Handle.hpp"

class Win32Thread : public Win32Handle
{
public:
	Win32Thread(HANDLE handle);
	~Win32Thread();

	void Wait(std::chrono::milliseconds timeout = std::chrono::milliseconds(INFINITE)) const;

	DWORD ExitCode();
};