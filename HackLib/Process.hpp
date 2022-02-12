#pragma once

#include "AutoHandle.hpp"
#include "NonCopyable.hpp"
#include "Exceptions.hpp"

class Process
{
public:
	Process(DWORD pid);
	Process(std::wstring_view name);
	NonCopyable(Process);
	virtual ~Process() = default;

	BYTE* BaseAddress() const;

	template<typename T>
	T Read(BYTE* pointer) const
	{
		T value;
		SIZE_T bytesRead = 0;

		if (!ReadProcessMemory(_handle, pointer, &value, sizeof(T), &bytesRead))
		{
			throw Win32Exception("ReadProcessMemory");
		}

		_ASSERT_EXPR(sizeof(T) == bytesRead, "ReadProcessMemory size mismatch!");
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

		_ASSERT_EXPR(sizeof(T) == bytesWritten, "WriteProcessMemory size mismatch!");
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