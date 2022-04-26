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

// TODO: consider switching to std::fstream...
class File : public Handle
{
public:
	File(
		const std::filesystem::path& path,
		DWORD desiredAccess = GENERIC_READ,
		DWORD shareMode = FILE_SHARE_READ,
		DWORD creationDisposition = OPEN_EXISTING,
		DWORD flagsAndAttributes = FILE_ATTRIBUTE_NORMAL) :
			Handle(CreateFileW(
				path.wstring().c_str(),
				desiredAccess,
				shareMode,
				nullptr,
				creationDisposition,
				flagsAndAttributes,
				nullptr))
	{
		if (!IsValid())
		{
			throw Win32Exception("CreateFile");
		}
	}

	inline bool Read(LPVOID buffer, size_t numberOfBytesToRead, LPDWORD numberOfBytesRead) const
	{
		return ReadFile(_handle, buffer, static_cast<DWORD>(numberOfBytesToRead), numberOfBytesRead, nullptr);
	}

	uint64_t Size() const
	{
		LARGE_INTEGER fileSize = {};

		if (!GetFileSizeEx(_handle, &fileSize) || fileSize.QuadPart < 0)
		{
			throw Win32Exception("GetFileSizeEx");
		}

		return static_cast<uint64_t>(fileSize.QuadPart);
	}
};