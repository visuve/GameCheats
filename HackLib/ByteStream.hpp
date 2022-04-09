#pragma once

#include "Pointer.hpp"

#include <array>
#include <span>

class ByteStream
{
public:
	ByteStream() = default;
	explicit ByteStream(size_t size, uint8_t byte = 0x00);
	explicit ByteStream(std::span<uint8_t> data);
	explicit ByteStream(std::initializer_list<uint8_t> data);

	template <size_t N>
	explicit  ByteStream(const std::array<uint8_t, N> data) :
		_bytes(data.begin(), data.end())
	{
	}

	ByteStream& operator << (const uint8_t byte);
	ByteStream& operator << (std::span<uint8_t> data);
	ByteStream& operator << (const Pointer& ptr);

	template <size_t N>
	ByteStream& operator << (const std::array<uint8_t, N>& data)
	{
		std::copy(data.cbegin(), data.cend(), std::back_inserter(_bytes));
		return *this;
	}

	template <size_t N>
	ByteStream& operator << (const uint8_t(&data)[N])
	{
		std::copy(std::cbegin(data), std::cend(data), std::back_inserter(_bytes));
		return *this;
	}

	static ByteStream FromString(const std::string& raw);

	void Fill(size_t n, uint8_t byte);

	operator std::span<uint8_t>();

	uint8_t& operator [](size_t i);

	uint8_t* Data();
	size_t Size() const;

private:
	std::vector<uint8_t> _bytes;
};