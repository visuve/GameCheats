#pragma once

#include "Exceptions.hpp"
#include "Logger.hpp"
#include "NonCopyable.hpp"
#include "PointerMap.hpp"
#include "VirtualMemory.hpp"
#include "Win32Process.hpp"
#include "Win32Thread.hpp"

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
		size_t bytesRead = _targetProcess.ReadProcessMemory(pointer, value, size);
		_ASSERT_EXPR(bytesRead == size, L"ReadProcessMemory size mismatch!");
		Log << "Read" << bytesRead << "bytes from" << pointer ;
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
		size_t bytesWritten = _targetProcess.WriteProcessMemory(pointer, value, size);
		_ASSERT_EXPR(bytesWritten == size, L"WriteProcessMemory size mismatch!");
		Log << "Wrote" << bytesWritten << "bytes at" << pointer ;
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
				address.Value(),
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

	void WaitForIdle();

	bool Verify(std::string_view expectedSHA256) const;

	MODULEENTRY32W FindModule(std::wstring_view name) const;

	IMAGE_NT_HEADERS NtHeader() const;

	IMAGE_IMPORT_DESCRIPTOR FindImport(std::string_view moduleName) const;

	Pointer FindFunction(IMAGE_IMPORT_DESCRIPTOR iid, std::string_view functionName) const;
	Pointer FindFunction(std::string_view moduleName, std::string_view functionName) const;

	Pointer AllocateMemory(size_t size);

	template <typename... T>
	PointerMap AllocateMap(T&& ... names)
	{
		constexpr size_t sizeNeeded = sizeof ... (T) * Pointer::Size;
		const Pointer region = AllocateMemory(sizeNeeded);
		return PointerMap(region, { std::forward<T>(names)... });
	}

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
	Pointer InjectX64(Pointer origin, size_t nops, std::span<uint8_t> code);
	Pointer InjectX64(size_t offset, size_t nops, std::span<uint8_t> code);
	Pointer InjectX64(std::wstring_view module, size_t offset, size_t nops, std::span<uint8_t> code);

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
	Pointer InjectX86(Pointer origin, size_t nops, std::span<uint8_t> code);
	Pointer InjectX86(size_t offset, size_t nops, std::span<uint8_t> code);
	Pointer InjectX86(std::wstring_view module, size_t offset, size_t nops, std::span<uint8_t> code);

	static constexpr size_t JumpOpSize = 5;

	inline static std::array<uint8_t, JumpOpSize> JumpOp(Pointer from, Pointer to)
	{
		_ASSERT_EXPR(from != to, L"Jump to nowhere");

		Pointer dst(to - from);
		dst -= JumpOpSize;

		return { 0xE9, dst[0], dst[1], dst[2], dst[3] };
	}

	inline static std::array<uint8_t, CallOpSize> CallOp(Pointer to)
	{
		return { 0xFF, 0x15, to[0],	to[1], to[2], to[3] };
	}
#endif

	DWORD WairForExit(std::chrono::milliseconds = std::chrono::milliseconds(INFINITE));

private:
	DWORD _pid = 0;
	MODULEENTRY32W _module = {};
	Win32Process _targetProcess;
	std::set<Win32Thread> _threads;
	std::set<VirtualMemory> _regions;
};
