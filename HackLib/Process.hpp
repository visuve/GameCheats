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
	void Write(BYTE* pointer, T value, SIZE_T size = sizeof(T)) const
	{
		SIZE_T bytesWritten = 0;

		if (!WriteProcessMemory(_handle, pointer, &value, size, &bytesWritten))
		{
			throw Win32Exception("WriteProcessMemory");
		}

		_ASSERT_EXPR(bytesWritten == size, L"WriteProcessMemory size mismatch!");
	}

	template<typename T, size_t N>
	void Fill(BYTE* pointer, T value) const
	{
		T data[N];
		std::memset(data, value, sizeof(data));
		Write(pointer, data, sizeof(data));
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