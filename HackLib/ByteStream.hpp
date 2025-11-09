#pragma once

#include "Pointer.hpp"

class ByteStream
{
public:
	ByteStream() = default;
	explicit ByteStream(size_t size, uint8_t byte = 0x00);
	explicit ByteStream(std::span<const uint8_t> data);
	explicit ByteStream(std::initializer_list<uint8_t> data);
	explicit ByteStream(std::string&& data);
	explicit ByteStream(const Pointer& ptr);

	ByteStream& operator << (const uint8_t byte);
	ByteStream& operator << (std::span<const uint8_t> data);
	ByteStream& operator << (const Pointer& ptr);

	template <size_t N>
	ByteStream& operator << (const uint8_t(&data)[N])
	{
		std::copy(std::cbegin(data), std::cend(data), std::back_inserter(_bytes));
		return *this;
	}
	ByteStream& operator << (std::string&& bytes);

	void Add(size_t n, uint8_t byte);

	bool operator == (const ByteStream& other) const;
	
	inline operator std::span<uint8_t>()
	{
		return _bytes;
	}

	uint8_t& operator [](size_t i);
	uint8_t operator [](size_t i) const;

	uint8_t* Data();
	size_t Size() const;

	inline auto begin() const
	{
		return _bytes.begin();
	}

	inline auto end() const
	{
		return _bytes.end();
	}

	operator std::string() const;

	void Replace(ByteStream from, ByteStream to);

private:
	std::vector<uint8_t> _bytes;
};

std::ostream& operator << (std::ostream&, const ByteStream&);