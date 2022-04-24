#pragma once

union Pointer
{
	constexpr static size_t Size = sizeof(void*);
	uint8_t* Value = nullptr;
	uint8_t Bytes[Size];

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

	inline uint8_t& operator[](size_t n)
	{
		if (n >= Size)
		{
			throw OutOfRangeException("index is out of bounds");
		}

		return Bytes[n];
	}

	inline const uint8_t operator[](size_t n) const
	{
		if (n >= Size)
		{
			throw OutOfRangeException("index is out of bounds");
		}

		return Bytes[n];
	}

	inline uint8_t* begin()
	{
		return &Bytes[0];
	}

	inline uint8_t* end()
	{
		return &Bytes[Size];
	}

	inline const uint8_t* cbegin() const
	{
		return &Bytes[0];
	}

	inline const uint8_t* cend() const
	{
		return &Bytes[Size];
	}
};

inline std::ostream& operator << (std::ostream& os, const Pointer& p)
{
	return os << "0x" << static_cast<void*>(p.Value);
}