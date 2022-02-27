#include "HackLib-PCH.hpp"
#include "ByteStream.hpp"

ByteStream::ByteStream(std::span<uint8_t> data) :
	_bytes(data.begin(), data.end())
{
}

ByteStream& ByteStream::operator << (const uint8_t byte)
{
	_bytes.emplace_back(byte);
	return *this;
}

ByteStream& ByteStream::operator << (std::span<uint8_t> data)
{
	std::copy(data.begin(), data.end(), std::back_inserter(_bytes));
	return *this;
}

ByteStream& ByteStream::operator << (const Pointer& ptr)
{
	std::copy(
		std::cbegin(ptr.Bytes),
		std::cend(ptr.Bytes),
		std::back_inserter(_bytes));

	return *this;
}

void ByteStream::Fill(size_t n, uint8_t byte)
{
	std::fill_n(std::back_inserter(_bytes), n, byte);
}

ByteStream::operator std::span<uint8_t>()
{
	return _bytes;
}

uint8_t& ByteStream::operator [](size_t i)
{
	return _bytes[i];
}

size_t ByteStream::Size() const
{
	return _bytes.size();
}
