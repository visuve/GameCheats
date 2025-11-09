#include "ByteStream.hpp"

ByteStream::ByteStream(size_t size, uint8_t byte) :
	_bytes(size, byte)
{
}

ByteStream::ByteStream(std::span<const uint8_t> data) :
	_bytes(data.begin(), data.end())
{
}

ByteStream::ByteStream(std::initializer_list<uint8_t> data) :
	_bytes(data)
{
}

ByteStream::ByteStream(std::string&& data)
{
	*this << std::move(data);
}

ByteStream::ByteStream(const Pointer& ptr) :
	_bytes(ptr.cbegin(), ptr.cend())
{
}

ByteStream& ByteStream::operator << (const uint8_t byte)
{
	_bytes.emplace_back(byte);
	return *this;
}

ByteStream& ByteStream::operator << (std::span<const uint8_t> data)
{
	std::copy(data.begin(), data.end(), std::back_inserter(_bytes));
	return *this;
}

ByteStream& ByteStream::operator << (const Pointer& ptr)
{
	std::copy(ptr.cbegin(), ptr.cend(), std::back_inserter(_bytes));
	return *this;
}

ByteStream& ByteStream::operator << (std::string&& bytes)
{
	std::stringstream stream(bytes);

	std::string raw;

	constexpr auto notDigit = [](char c)->bool
	{
		return !std::isxdigit(c);
	};

	while (std::getline(stream, raw, ' '))
	{
		if (raw.length() != 2)
		{
			throw ArgumentException(
				"Arguments have to be between 00-FF (zero padded) and separated with space");
		}

		if (std::any_of(raw.cbegin(), raw.cend(), notDigit))
		{
			throw ArgumentException("Only hexadecimal characters accepted");
		}

		int value = std::stoi(raw, nullptr, 16);

		if (!std::in_range<uint8_t>(value))
		{
			throw ArgumentException("A byte has to be between 0-255");
		}

		_bytes.push_back(static_cast<uint8_t>(value));
	}

	return *this;
}

void ByteStream::Add(size_t n, uint8_t byte)
{
	std::fill_n(std::back_inserter(_bytes), n, byte);
}

bool ByteStream::operator == (const ByteStream& other) const
{
	return _bytes == other._bytes;
}

uint8_t& ByteStream::operator [](size_t i)
{
	if (i >= _bytes.size())
	{
		throw OutOfRangeException("Index is out of bounds");
	}

	return _bytes[i];
}

uint8_t ByteStream::operator [](size_t i) const
{
	if (i >= _bytes.size())
	{
		throw OutOfRangeException("Index is out of bounds");
	}

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

ByteStream::operator std::string() const
{
	const size_t required = (_bytes.size() * 2) + (_bytes.size() - 1);
	std::string result(required, ' ');

	std::string::iterator it = result.begin();

	for (uint8_t byte : _bytes)
	{
		it = std::format_to(it, "{:02X}", byte);

		if (it != result.end())
		{
			++it; // Skip the space
		}
	}

	return result;
}

void ByteStream::Replace(ByteStream from, ByteStream to)
{
	if (from.Size() != to.Size())
	{
		throw ArgumentException("Replacement needs to equal in size");
	}

	auto it = std::ranges::search(_bytes, from);

	while (!it.empty())
	{
		std::copy(to.begin(), to.end(), it.begin());
		it = std::ranges::search(_bytes, from);
	}
}

std::ostream& operator << (std::ostream& os, const ByteStream& bs)
{
	return os << std::string(bs);
}