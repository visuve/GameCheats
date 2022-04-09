#include "HackLib-PCH.hpp"
#include "ByteStream.hpp"
#include "Exceptions.hpp"

ByteStream::ByteStream(size_t size, uint8_t byte) :
	_bytes(size, byte)
{
}

ByteStream::ByteStream(std::span<uint8_t> data) :
	_bytes(data.begin(), data.end())
{
}

ByteStream::ByteStream(std::initializer_list<uint8_t> data) :
	_bytes(data)
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

ByteStream ByteStream::FromString(const std::string& raw)
{
	std::stringstream stream(raw);
	stream.setf(std::ios::hex, std::ios::basefield);

	uint16_t value;
	ByteStream result;

	while (stream.good())
	{
		stream >> value;

		if (value > 0xFF)
		{
			throw LogicException("A single byte cannot be over 255!");
		}

		result << value;
	}

	return result;
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

uint8_t* ByteStream::Data()
{ 
	return _bytes.data();
}

size_t ByteStream::Size() const
{
	return _bytes.size();
}
