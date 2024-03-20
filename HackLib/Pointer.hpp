#pragma once

#include "Exceptions.hpp"

class Pointer
{
public:
	constexpr static size_t Size = sizeof(void*);

	constexpr Pointer() = default;

	template <typename T>
	constexpr Pointer(T value) requires std::unsigned_integral<T>&& std::convertible_to<T, size_t>
	{
		_internal.Value = value;
	}

	template <typename T>
	constexpr Pointer(const T* value) requires std::is_unsigned<T>::value || std::is_void<T>::value
	{
		_internal.Value = reinterpret_cast<size_t>(value);
	}

	constexpr Pointer(const Pointer& other) :
		_internal(other._internal)
	{
	}

	constexpr Pointer(Pointer&& other) noexcept:
		_internal(std::move(other._internal))
	{
	}

	inline virtual ~Pointer() = default;

	inline Pointer& operator = (const Pointer& p)
	{
		_internal.Value = p._internal.Value;
		return *this;
	}

	inline Pointer& operator = (Pointer&& p)
	{
		_internal.Value = p._internal.Value;
		return *this;
	}

	constexpr Pointer& operator += (const Pointer& p)
	{
		_internal.Value += p._internal.Value;
		return *this;
	}

	constexpr Pointer& operator -= (const Pointer& p)
	{
		_internal.Value -= p._internal.Value;
		return *this;
	}

	inline Pointer operator + (const Pointer& p) const
	{
		return _internal.Value + p._internal.Value;
	}

	inline Pointer operator - (const Pointer& p) const
	{
		return _internal.Value - p._internal.Value;
	}

	constexpr Pointer& operator += (size_t offset)
	{
		_internal.Value += offset;
		return *this;
	}

	constexpr Pointer& operator -= (size_t offset)
	{
		_internal.Value -= offset;
		return *this;
	}

	inline Pointer operator + (size_t offset) const
	{
		return _internal.Value + offset;
	}

	inline Pointer operator - (size_t offset) const
	{
		return _internal.Value - offset;
	}

	constexpr Pointer& operator ++ ()
	{
		_internal.Value += Size;
		return *this;
	}

	constexpr Pointer& operator -- ()
	{
		_internal.Value -= Size;
		return *this;
	}

	inline Pointer operator ++ (int)
	{
		Pointer tmp = *this;
		_internal.Value += Size;
		return tmp;
	}

	inline Pointer operator -- (int)
	{
		Pointer tmp = *this;
		_internal.Value -= Size;
		return tmp;
	}

	constexpr bool operator == (const Pointer& other) const
	{
		return _internal.Value == other._internal.Value;
	}

	constexpr auto operator <=> (const Pointer& other) const
	{
		return _internal.Value <=> other._internal.Value;
	}

	constexpr bool operator == (size_t offset) const
	{
		return _internal.Value == offset;
	}

	constexpr auto operator <=> (size_t offset) const
	{
		return _internal.Value <=> offset;
	}

	inline const uint8_t* Value() const
	{
		return reinterpret_cast<uint8_t*>(_internal.Value);
	}

	inline operator void* () const
	{
		return reinterpret_cast<void*>(_internal.Value);
	}

	inline uint8_t& operator[](size_t n)
	{
		if (n >= Size)
		{
			throw OutOfRangeException("index is out of bounds");
		}

		return _internal.Bytes[n];
	}

	constexpr const uint8_t operator[](size_t n) const
	{
		if (n >= Size)
		{
			throw OutOfRangeException("index is out of bounds");
		}

		return _internal.Bytes[n];
	}

	constexpr uint8_t* begin()
	{
		return &_internal.Bytes[0];
	}

	constexpr uint8_t* end()
	{
		return &_internal.Bytes[Size];
	}

	constexpr const uint8_t* cbegin() const
	{
		return &_internal.Bytes[0];
	}

	constexpr const uint8_t* cend() const
	{
		return &_internal.Bytes[Size];
	}
	
	friend std::formatter<Pointer>;

private:
	union Internal
	{
		size_t Value = 0;
		uint8_t Bytes[Size];
	} _internal;
};

template <>
struct std::formatter<Pointer>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const Pointer& p, std::format_context& ctx) const
	{
		if constexpr (Pointer::Size == 8)
		{
			return std::format_to(ctx.out(), "0x{:016X}", p._internal.Value);
		}
		else
		{
			return std::format_to(ctx.out(), "0x{:08X}", p._internal.Value);
		}
	}
};

inline std::ostream& operator << (std::ostream& os, const Pointer& p)
{
	return os << std::format("{0}", p);
}
