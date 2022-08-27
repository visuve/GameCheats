#pragma once

class Win32Handle
{
public:
	Win32Handle();
	explicit Win32Handle(HANDLE handle);
	Win32Handle(const Win32Handle& other) = delete;
	Win32Handle(Win32Handle&& other);
	virtual ~Win32Handle();

	Win32Handle& operator = (HANDLE handle);
	Win32Handle& operator = (const Win32Handle& other) = delete;
	Win32Handle& operator = (Win32Handle&& other);

	bool IsValid() const;
	operator bool() const;

	void Reset(HANDLE handle = nullptr);

protected:
	HANDLE _handle = nullptr;
};