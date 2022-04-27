#pragma once

#include "System.hpp"
#include "Win32Handle.hpp"

class Win32Event : public Win32Handle
{
public:
	Win32Event(const std::string& name = System::GenerateGuid());
	~Win32Event();

	DWORD Wait(std::chrono::milliseconds timeout = std::chrono::milliseconds(INFINITE)) const;

	HANDLE Value() const;

private:
	const std::string _name;
};