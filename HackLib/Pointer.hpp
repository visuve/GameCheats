#pragma once

#include <iomanip>

#ifdef _WIN64
constexpr size_t PointerSizeBytes = 8;
#else
constexpr size_t PointerSizeBytes = 4;
#endif

union Pointer
{
	uint8_t* Value = nullptr;
	uint8_t Bytes[PointerSizeBytes];

	inline Pointer() = default;

	inline Pointer(uint8_t* value) :
		Value(value)
	{
	}

	inline Pointer(const Pointer& other) :
		Value(other.Value)
	{
	}

	inline Pointer& operator = (const Pointer& p)
	{
		Value = p.Value;
		return *this;
	}

	inline Pointer& operator += (const Pointer& p)
	{
		Value += reinterpret_cast<size_t>(p.Value);
		return *this;
	}

	inline Pointer& operator -= (const Pointer& p) 
	{
		Value -= reinterpret_cast<size_t>(p.Value);
		return *this;
	}

	inline Pointer operator + (const Pointer& p) const
	{
		return Value + reinterpret_cast<size_t>(p.Value);
	}

	inline Pointer operator - (const Pointer& p) const
	{
		return Value - reinterpret_cast<size_t>(p.Value);
	}

	inline Pointer& operator += (size_t offset)
	{
		Value += offset;
		return *this;
	}

	inline Pointer& operator -= (size_t offset)
	{
		Value -= offset;
		return *this;
	}

	inline Pointer operator + (size_t offset) const
	{
		return Value + offset;
	}

	inline Pointer operator - (size_t offset) const
	{
		return Value - offset;
	}

	inline bool operator == (const Pointer& other) const
	{
		return Value == other.Value;
	}

	inline bool operator != (const Pointer& other) const
	{
		return Value != other.Value;
	}

	inline bool operator < (const Pointer& other) const
	{
		return Value < other.Value;
	}

	inline bool operator > (const Pointer& other) const
	{
		return Value > other.Value;
	}

	inline bool operator < (size_t offset) const
	{
		return reinterpret_cast<size_t>(Value) < offset;
	}

	inline bool operator > (size_t offset) const
	{
		return reinterpret_cast<size_t>(Value) > offset;
	}

	inline operator void* () const
	{
		return Value;
	}

	inline uint8_t operator[](size_t n) const
	{
		_ASSERT(n < PointerSizeBytes);
		return Bytes[n];
	}
};

inline std::ostream& operator << (std::ostream& os, const Pointer& p)
{
	std::ios_base::fmtflags formatFlags = os.flags();

	os.setf(std::ios::hex, std::ios::basefield);
	os.setf(std::ios::uppercase);
	os.fill('0');
	os << "0x";

	for (auto it = std::rbegin(p.Bytes); it != std::rend(p.Bytes); ++it)
	{
		os << std::setw(2) << uint16_t(*it);
	}

	os.flags(formatFlags);

	return os;
}