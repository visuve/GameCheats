#pragma once


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

	inline Pointer operator + (const Pointer& p)
	{
		return Value + reinterpret_cast<size_t>(p.Value);
	}

	inline Pointer operator - (const Pointer& p)
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

	inline Pointer operator + (size_t offset)
	{
		return Value + offset;
	}

	inline Pointer operator - (size_t offset)
	{
		return Value - offset;
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