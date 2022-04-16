#pragma once

#include "Exceptions.hpp"
#include "NonCopyable.hpp"
#include "Pointer.hpp"

class Process
{
public:
	Process(DWORD pid, bool waitForExit = false);
	Process(std::wstring_view name, bool waitForExit = false);
	NonCopyable(Process);
	virtual ~Process();

	inline Pointer Address(size_t offset) const
	{
		return { _module.modBaseAddr + offset };
	}

	inline Pointer Address(std::wstring_view module, size_t offset) const
	{
		return FindModule(module).modBaseAddr + offset;
	}

	template<typename ... T>
	Pointer ResolvePointer(size_t base, T ... offsets) const
	{
		Pointer pointer = Address(base);

		for (size_t offset : { offsets... })
		{
			pointer = Read<Pointer>(pointer) + offset;
		}

		_ASSERT_EXPR(pointer, L"The pointer is null!");

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

		_ASSERT_EXPR(bytesRead == size, L"ReadProcessMemory size mismatch!");

		std::cout << "Read " << bytesRead << " bytes from " << pointer << std::endl;
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
	void Write(Pointer pointer, const T* value, size_t size) const
	{
		SIZE_T bytesWritten = 0;

		if (!WriteProcessMemory(_handle, pointer, value, size, &bytesWritten))
		{
			throw Win32Exception("WriteProcessMemory");
		}

		_ASSERT_EXPR(bytesWritten == size, L"WriteProcessMemory size mismatch!");

		std::cout << "Wrote " << bytesWritten << " bytes at " << pointer << std::endl;
	}

	template<typename T>
	void Write(Pointer pointer, const T& value) const
	{
		Write(pointer, &value, sizeof(value));
	}
	
	template<typename T>
	void Write(size_t offset, const T& value) const
	{
		Write(Address(offset), value);
	}

	inline void WriteBytes(Pointer pointer, std::span<uint8_t> bytes) const
	{
		Write(pointer, bytes.data(), bytes.size_bytes());
	}

	inline void WriteBytes(size_t offset, std::span<uint8_t> bytes) const
	{
		WriteBytes(Address(offset), bytes);
	}

	template<typename T>
	void Fill(size_t from, size_t to, T value) const
	{
		const size_t bytes = to - from;
		const size_t elements = bytes / sizeof(T);
		
		if (bytes % sizeof(T))
		{
			throw LogicException("Alignment mismatch!");
		}

		Pointer pointer = Address(from);
		std::vector<T> filler(elements, value);

		Write(pointer, filler.data(), bytes);
	}

	inline void ChangeByte(Pointer address, uint8_t from, uint8_t to)
	{
		const uint8_t current = Read<uint8_t>(address);

		if (current != from)
		{
			char message[0x40] = {};

			std::snprintf(
				message,
				sizeof(message) - 1,
				"Error @ %p. Expected 0x%02X, got 0x%02X.",
				address.Value,
				from,
				current);

			throw LogicException(message);
		}

		Write<uint8_t>(address, to);
	}

	inline void ChangeByte(size_t offset, uint8_t from, uint8_t to)
	{
		ChangeByte(Address(offset), from, to);
	}

	MODULEENTRY32W FindModule(std::wstring_view name) const;

	IMAGE_NT_HEADERS NtHeader() const;

	IMAGE_IMPORT_DESCRIPTOR FindImport(std::string_view moduleName) const;

	Pointer FindFunction(std::string_view moduleName, std::string_view functionName) const;

	Pointer AllocateMemory(size_t size);

	DWORD CreateThread(Pointer address, Pointer parameter, bool detached = false);

	DWORD InjectLibrary(std::string_view name);

	// NOTE: all memory is freed in ~Process(),
	// hence this is needed in rare cases only
	void FreeMemory(Pointer pointer);

	static constexpr size_t CallOpSize = 6;

	// NOTE: In x64, VirtualAllocEx tends to allocate memory
	// so far away, that a relative "x86 jump" will not do.
	// In x86 I could not get absolute jumps to work.
#ifdef _WIN64
	Pointer InjectX64(size_t offset, size_t nops, std::span<uint8_t> code);

	static constexpr size_t JumpOpSize = 14;

	inline static std::array<uint8_t, JumpOpSize> JumpAbsolute(Pointer ptr)
	{
		return 
		{ 
			0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
			ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7]
		};
	}

	inline std::array<uint8_t, JumpOpSize> JumpAbsolute(size_t offset)
	{
		return JumpAbsolute(Address(offset));
	}

	inline std::array<uint8_t, CallOpSize> CallOp(size_t from, Pointer to)
	{
		Pointer dst(to - (Address(from) + CallOpSize));
		_ASSERT_EXPR(dst < 0xFFFFFFFF, L"Will not fit in a call op");
		return { 0xFF, 0x15, dst[0], dst[1], dst[2], dst[3] };
	}
#else
	Pointer InjectX86(size_t offset, size_t nops, std::span<uint8_t> code);

	static constexpr size_t JumpOpSize = 5;

	inline static std::array<uint8_t, JumpOpSize> JumpOp(Pointer from, Pointer to)
	{
		_ASSERT_EXPR(from != to, L"Jump to nowhere");

		if (from < to)
		{
			from += JumpOpSize;
		}

		Pointer dst(to - from);

		return { 0xE9, dst[0], dst[1], dst[2], dst[3] };
	}

	inline static std::array<uint8_t, CallOpSize> CallOp(Pointer to)
	{
		return { 0xFF, 0x15, to[0],	to[1], to[2], to[3] };
	}
#endif

private:
	DWORD _pid = 0;
	MODULEENTRY32W _module = {};
	HANDLE _handle = nullptr;
	bool _waitForExit = false;
	std::set<HANDLE> _threads;
	std::set<Pointer> _memory;
};
