#pragma once

#include "NonCopyable.hpp"
#include "Win32Handle.hpp"

class Win32File : public Win32Handle
{
public:
	explicit Win32File(const std::filesystem::path& path, DWORD access = GENERIC_READ);
	virtual ~Win32File();

	NonCopyable(Win32File);

	inline HANDLE Value() const
	{
		return _handle;
	}

	inline size_t Size() const
	{
		return _size;
	}

	inline operator bool() const
	{
		return _ovl.Offset <= _size;
	}

	size_t Read(void* buffer, size_t size);
	size_t ReadAt(void* buffer, size_t size, size_t offset);

	template<std::semiregular T, size_t N = sizeof(T)>
	T Read()
	{
		T result = {};

		[[maybe_unused]]
		size_t bytesRead = Read(&result, N);

		_ASSERT(bytesRead == N);
		return result;
	}

	template<std::semiregular T, size_t N = sizeof(T)>
	T ReadAt(size_t offset)
	{
		T result = {};

		[[maybe_unused]]
		size_t bytesRead = ReadAt(&result, N, offset);

		_ASSERT(bytesRead == N);
		return result;
	}

	std::string ReadUntil(char byte);
	std::string ReadAtUntil(size_t offset, char byte);

	size_t Write(void* buffer, size_t size);
	size_t WriteAt(void* buffer, size_t size, size_t offset);

	template<std::semiregular T, size_t N = sizeof(T)>
	void Write(T x)
	{
		[[maybe_unused]]
		size_t bytesWritten = Write(&x, N);
		_ASSERT(bytesWritten == N);
	}

	template<std::semiregular T, size_t N = sizeof(T)>
	void WriteAt(T x, size_t offset)
	{
		[[maybe_unused]]
		size_t bytesWritten = WriteAt(&x, N, offset);
		_ASSERT(bytesWritten == N);
	}

	size_t CurrentPosition() const;
	void SetPosition(size_t);

private:
	size_t _size = 0;
	OVERLAPPED _ovl = {};
};