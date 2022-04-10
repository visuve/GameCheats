#include "../Mega.pch"

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

ByteStream::ByteStream(const std::string& data)
{
	*this << data;
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

ByteStream& ByteStream::operator << (const std::string& bytes)
{
	std::stringstream stream(bytes);
	stream.setf(std::ios::hex, std::ios::basefield);

	uint16_t value;

	while (stream.good())
	{
		stream >> value;

		if (value > 0xFF)
		{
			throw LogicException("A single byte cannot be over 255!");
		}

		*this << static_cast<uint8_t>(value);
	}

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

uint8_t* ByteStream::Data()
{ 
	return _bytes.data();
}

size_t ByteStream::Size() const
{
	return _bytes.size();
}

std::ostream& operator << (std::ostream& os, const ByteStream& bs)
{
	std::ios_base::fmtflags formatFlags = os.flags();

	os.setf(std::ios::hex, std::ios::basefield);
	os.setf(std::ios::uppercase);
	os.fill('0');

	const std::string sep1 = " ";
	std::string sep2 = "";

	for (uint16_t byte : bs._bytes)
	{
		os << sep2 << std::setw(2) << byte;
		sep2 = sep1;
	}

	os.flags(formatFlags);
	
	return os;
}