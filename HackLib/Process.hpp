#pragma once

#include "NonCopyable.hpp"
#include "Exceptions.hpp"
#include "Pointer.hpp"
#include "OpCodes.hpp"

#define NOMINMAX
#define VC_EXTRALEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <span>
#include <set>

class Process
{
public:
	Process(DWORD pid);
	Process(std::wstring_view name);
	NonCopyable(Process);
	virtual ~Process();

	inline Pointer Address(size_t offset) const
	{
		return { _module.modBaseAddr + offset };
	}

	template<typename ... T>
	Pointer ResolvePointer(size_t base, T ... offsets) const
	{
		Pointer pointer = Address(base);

		for (size_t offset : { offsets... })
		{
			pointer = Read<Pointer>(pointer) + offset;
		}

		_ASSERT_EXPR(pointer, "The pointer is null!");

		return pointer;
	}

	template<typename T>
	void Read(Pointer pointer, T* value, size_t size) const
	{
		SIZE_T bytesRead = 0;

		if (!ReadProcessMemory(_handle, pointer, value, size, &bytesRead))
		{
			throw Win32Exception("ReadProcessMemory");
		}

		_ASSERT_EXPR(bytesRead == size, "ReadProcessMemory size mismatch!");
	}

	template<typename T>
	T Read(Pointer pointer) const
	{
		T value = {};
		Read(pointer, &value, sizeof(T));
		return value;
	}

	template<typename T>
	T Read(size_t offset) const
	{
		return Read<T>(Address(offset));
	}

	template<typename T>
	void Write(Pointer pointer, const T& value) const
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

	inline void WriteBytes(Pointer pointer, std::span<uint8_t> bytes) const
	{
		SIZE_T bytesWritten = 0;

		if (!WriteProcessMemory(_handle, pointer, bytes.data(), bytes.size_bytes(), &bytesWritten))
		{
			throw Win32Exception("WriteProcessMemory");
		}

		_ASSERT_EXPR(bytesWritten == bytes.size_bytes(), L"WriteProcessMemory size mismatch!");
	}

	template<size_t Start, size_t End, typename T>
	void Fill(T value) const
	{
		constexpr size_t bytes = End - Start;
		constexpr size_t elements = bytes / sizeof(T);
		static_assert(bytes % sizeof(T) == 0);

		T data[elements];
		std::memset(data, value, bytes);
		
		Pointer pointer = Address(Start);
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

	MODULEENTRY32W FindModule(std::wstring_view name) const;

	IMAGE_NT_HEADERS NtHeader() const;

	IMAGE_IMPORT_DESCRIPTOR FindImport(std::string_view moduleName) const;

	Pointer FindFunctionPointer(std::string_view moduleName, std::string_view functionName) const;

	Pointer AllocateMemory(size_t size);

	void CreateThread(Pointer address);

	// NOTE: all memory is freed in ~Process(),
	// hence this is needed in rare cases only
	void FreeMemory(Pointer pointer);

	// NOTE: In x64, VirtualAllocEx tends to allocate memory
	// so far away, that a relative "x86 jump" will not do.
	// In x86 I could not get absolute jumps to work.
#ifdef _WIN64
	void InjectX64(size_t offset, std::span<uint8_t> code);
#else
	void InjectX86(size_t offset, std::span<uint8_t> code);
#endif

private:
	DWORD _pid = 0;
	MODULEENTRY32W _module = {};
	HANDLE _handle = nullptr;
	std::set<HANDLE> _threads;
	std::set<Pointer> _memory;
};
