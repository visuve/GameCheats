#pragma once

#include "NonCopyable.hpp"
#include "Exceptions.hpp"

#define NOMINMAX
#define VC_EXTRALEAN
#include <Windows.h>
#include <TlHelp32.h>

class Process
{
public:
	Process(DWORD pid);
	Process(std::wstring_view name);
	NonCopyable(Process);
	virtual ~Process();

	uint8_t* Address(size_t offset) const;

	template<typename ... T>
	uint8_t* ResolvePointer(size_t base, T ... offsets) const
	{
		uint8_t* pointer = Address(base);

		for (size_t offset : { offsets... })
		{
			pointer = Read<uint8_t*>(pointer) + offset;
		}

		_ASSERT_EXPR(pointer, "The pointer is null!");

		return pointer;
	}

	template<typename T>
	void Read(uint8_t* pointer, T* value, size_t size) const
	{
		SIZE_T bytesRead = 0;

		if (!ReadProcessMemory(_handle, pointer, value, size, &bytesRead))
		{
			throw Win32Exception("ReadProcessMemory");
		}

		_ASSERT_EXPR(bytesRead == size, "ReadProcessMemory size mismatch!");
	}

	template<typename T>
	T Read(uint8_t* pointer) const
	{
		T value = T();
		Read(pointer, &value, sizeof(T));
		return value;
	}

	template<typename T>
	T Read(size_t offset) const
	{
		return Read<T>(Address(offset));
	}

	template<typename T>
	void Write(uint8_t* pointer, const T& value) const
	{
		SIZE_T bytesWritten = 0;

		if (!WriteProcessMemory(_handle, pointer, &value, sizeof(value), &bytesWritten))
		{
			throw Win32Exception("WriteProcessMemory");
		}

		_ASSERT_EXPR(bytesWritten == sizeof(value), L"WriteProcessMemory size mismatch!");
	}
	
	template<typename T>
	void Write(size_t offset, const T& value) const
	{
		Write(Address(offset), value);
	}

	template<size_t N>
	void Write(uint8_t* pointer, const uint8_t(&bytes)[N]) const
	{
		SIZE_T bytesWritten = 0;

		if (!WriteProcessMemory(_handle, pointer, bytes, N, &bytesWritten))
		{
			throw Win32Exception("WriteProcessMemory");
		}

		_ASSERT_EXPR(bytesWritten == N, L"WriteProcessMemory size mismatch!");
	}

	template<size_t N>
	void Write(size_t offset, const uint8_t(&bytes)[N]) const
	{
		Write(Address(offset), bytes);
	}

	template<size_t Start, size_t End, typename T>
	void Fill(T value) const
	{
		constexpr size_t bytes = End - Start;
		constexpr size_t elements = bytes / sizeof(T);
		static_assert(bytes % sizeof(T) == 0);

		T data[elements];
		std::memset(data, value, bytes);
		
		uint8_t* pointer = Address(Start);
		SIZE_T bytesWritten = 0;

		if (!WriteProcessMemory(_handle, pointer, data, bytes, &bytesWritten))
		{
			throw Win32Exception("WriteProcessMemory");
		}

		_ASSERT_EXPR(bytesWritten == bytes, L"WriteProcessMemory size mismatch!");
	}

	template<size_t Offset>
	void ChangeByte(uint8_t from, uint8_t to)
	{
		const uint8_t current = Read<uint8_t>(Offset);

		if (current != from)
		{
			char message[0x40] = {};
			
			std::snprintf(
				message,
				sizeof(message) - 1,
				"Error @ 0x%zX. Expected 0x%02X, got 0x%02X.",
				Offset,
				from,
				current);

			throw LogicException(message);
		}
		
		Write<uint8_t>(Offset, to);
	}

private:
	DWORD _pid = 0;
	MODULEENTRY32W _module = {};
	HANDLE _handle = nullptr;
};