#pragma once

#include "AutoHandle.hpp"
#include "NonCopyable.hpp"
#include "Exceptions.hpp"

#include <TlHelp32.h>

class Process
{
public:
	Process(DWORD pid);
	Process(std::wstring_view name);
	NonCopyable(Process);
	virtual ~Process() = default;

	BYTE* BaseAddress() const;
	BYTE* Address(DWORD offset) const;

	template<typename T>
	void Read(BYTE* pointer, T& value, SIZE_T size = sizeof(T)) const
	{
		SIZE_T bytesRead = 0;

		if (!ReadProcessMemory(_handle, pointer, &value, size, &bytesRead))
		{
			throw Win32Exception("ReadProcessMemory");
		}

		_ASSERT_EXPR(bytesRead == size, "ReadProcessMemory size mismatch!");
	}

	template<typename T>
	T Read(BYTE* pointer) const
	{
		T value = T();
		Read(pointer, value);
		return value;
	}

	template<typename T>
	void Write(BYTE* pointer, T value) const
	{
		SIZE_T bytesWritten = 0;

		if (!WriteProcessMemory(_handle, pointer, &value, sizeof(T), &bytesWritten))
		{
			throw Win32Exception("WriteProcessMemory");
		}

		_ASSERT_EXPR(bytesWritten == sizeof(T), L"WriteProcessMemory size mismatch!");
	}

	template<typename T, DWORD start, DWORD end>
	void Fill(T value) const
	{
		constexpr SIZE_T bytes = end - start;
		constexpr SIZE_T elements = bytes / sizeof(T);
		static_assert(bytes % sizeof(T) == 0);

		T data[elements];
		std::memset(data, value, bytes);
		
		BYTE* pointer = Address(start);
		SIZE_T bytesWritten = 0;

		if (!WriteProcessMemory(_handle, pointer, data, bytes, &bytesWritten))
		{
			throw Win32Exception("WriteProcessMemory");
		}

		_ASSERT_EXPR(bytesWritten == bytes, L"WriteProcessMemory size mismatch!");
	}

	template<size_t N>
	BYTE* FindPointer(BYTE* pointer, const DWORD(&offsets)[N] ) const
	{
		for (DWORD offset : offsets)
		{
			pointer = Read<BYTE*>(pointer) + offset;
		}

		return pointer;
	}

private:
	DWORD _pid = 0;
	MODULEENTRY32W _module = {};
	AutoHandle _handle;
};