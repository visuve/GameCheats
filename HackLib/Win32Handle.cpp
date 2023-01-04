#include "Win32Handle.hpp"

Win32Handle::Win32Handle()
{
}

Win32Handle::Win32Handle(HANDLE handle) :
	_handle(handle)
{
}

Win32Handle::Win32Handle(Win32Handle&& other)
{
	if (this != &other)
	{
		std::swap(_handle, other._handle);
	}
}

Win32Handle::~Win32Handle()
{
	Reset(nullptr);
}

Win32Handle& Win32Handle::operator = (HANDLE handle)
{
	Reset(handle);
	return *this;
}

Win32Handle& Win32Handle::operator = (Win32Handle&& other)
{
	if (this != &other)
	{
		std::swap(_handle, other._handle);
	}

	return *this;
}

bool Win32Handle::IsValid() const
{
	return _handle != nullptr && 
		_handle != INVALID_HANDLE_VALUE;
}

Win32Handle::operator bool() const
{
	return IsValid();
}

void Win32Handle::Reset(HANDLE handle)
{
	if (_handle == handle)
	{
		return;
	}

	if (IsValid())
	{
		[[maybe_unused]]
		bool result = CloseHandle(_handle);
		_ASSERT(result);
	}

	_handle = handle;
}