#pragma once

#include "NonCopyable.hpp"
#include "Win32Handle.hpp"

class Win32File : public Win32Handle
{
public:
	explicit Win32File(const std::filesystem::path& path);
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

	size_t Read(void* buffer, size_t size) const;

	template<std::semiregular T, size_t N = sizeof(T)>
	T Read() const
	{
		T result = {};

		[[maybe_unused]]
		size_t bytesRead = Read(&result, N);

		_ASSERT(bytesRead == N);
		return result;
	}

	size_t ReadAt(void* buffer, size_t size, size_t offset) const;

	template<std::semiregular T, size_t N = sizeof(T)>
	T ReadAt(size_t offset) const
	{
		T result = {};

		[[maybe_unused]]
		size_t bytesRead = ReadAt(&result, N, offset);

		_ASSERT(bytesRead == N);
		return result;
	}

	std::string ReadUntil(char byte) const;
	std::string ReadAtUntil(size_t offset, char byte) const;

	size_t CurrentPosition() const;
	void SetPosition(size_t) const;

private:
	size_t _size = 0;
};