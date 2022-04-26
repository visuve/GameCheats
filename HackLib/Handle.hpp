#pragma once

#include "NonCopyable.hpp"
#include "Exceptions.hpp"

class Handle
{
public:
	inline Handle() = default;

	explicit inline Handle(HANDLE handle) :
		_handle(handle)
	{
	}

	inline Handle(const Handle& other) = delete;

	inline Handle(Handle&& other)
	{
		if (this != &other)
		{
			std::swap(_handle, other._handle);
		}
	}

	inline virtual ~Handle()
	{
		Reset(nullptr);
	}

	inline Handle& operator = (HANDLE handle)
	{
		Reset(handle);
		return *this;
	}

	inline Handle& operator = (const Handle& other) = delete;

	inline Handle& operator = (Handle&& other)
	{
		if (this != &other)
		{
			std::swap(_handle, other._handle);
		}

		return *this;
	}

	inline HANDLE Value() const
	{
		return _handle;
	}

	inline virtual bool IsValid() const
	{
		return _handle && _handle != INVALID_HANDLE_VALUE;
	}

	inline operator bool() const
	{
		return IsValid();
	}

	inline void Reset(HANDLE handle = nullptr)
	{
		if (_handle == handle)
		{
			return;
		}

		if (_handle)
		{
			bool result = CloseHandle(_handle);
			_ASSERT(result);
			(void)result;
		}

		_handle = handle;
	}

protected:
	HANDLE _handle = nullptr;
};