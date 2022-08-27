#pragma once

#include "NonCopyable.hpp"
#include "Win32Handle.hpp"

class Win32Event : public Win32Handle
{
	Win32Event(const std::string& name);
public:
	Win32Event();
	~Win32Event();

	NonCopyable(Win32Event);

	DWORD Wait(std::chrono::milliseconds timeout = std::chrono::milliseconds(INFINITE)) const;

	HANDLE Value() const;

private:
	const std::string _name;
};