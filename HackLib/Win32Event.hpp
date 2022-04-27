#pragma once

#include "Win32Handle.hpp"


class Win32Event : public Win32Handle
{
public:
	Win32Event(std::wstring_view name, bool consoleEvent);
	~Win32Event();

	DWORD Wait(std::chrono::milliseconds timeout = std::chrono::milliseconds(INFINITE)) const;

	void SetEvent() const;

	HANDLE Value() const;
};