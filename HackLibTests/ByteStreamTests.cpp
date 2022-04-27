#include "ByteStream.hpp"

TEST(ByteStreamTests, Constructors)
{
	{
		ByteStream stream(3, 0x90u);
		EXPECT_EQ(stream[0], 0x90u);
		EXPECT_EQ(stream[1], 0x90u);
		EXPECT_EQ(stream[2], 0x90u);
		EXPECT_EQ(stream.Size(), 3u);
	}
	{
		uint8_t bytes[] = { 4, 5, 6 };
		ByteStream stream(bytes);
		EXPECT_EQ(stream[0], 0x04u);
		EXPECT_EQ(stream[1], 0x05u);
		EXPECT_EQ(stream[2], 0x06u);
		EXPECT_EQ(stream.Size(), 3u);
	}
	{
		std::vector<uint8_t> data = { 4, 5, 6 };
		ByteStream stream(data);

		EXPECT_EQ(stream[0], 0x04u);
		EXPECT_EQ(stream[1], 0x05u);
		EXPECT_EQ(stream[2], 0x06u);
		EXPECT_EQ(stream.Size(), 3u);
	}
	{
		std::array<uint8_t, 3> data = { 4, 5, 6 };
		ByteStream stream(data);

		EXPECT_EQ(stream[0], 0x04u);
		EXPECT_EQ(stream[1], 0x05u);
		EXPECT_EQ(stream[2], 0x06u);
		EXPECT_EQ(stream.Size(), 3u);
	}
	{
		ByteStream stream({ 4, 5, 6 });
		EXPECT_EQ(stream[0], 0x04u);
		EXPECT_EQ(stream[1], 0x05u);
		EXPECT_EQ(stream[2], 0x06u);
		EXPECT_EQ(stream.Size(), 3u);
	}
}

TEST(ByteStreamTests, StreamOperators)
{
	{
		ByteStream stream;
#pragma warning(suppress: 4312)
		stream << Pointer(0xDEADBEEFu);

		EXPECT_EQ(stream[0], 0xEFu);
		EXPECT_EQ(stream[1], 0xBEu);
		EXPECT_EQ(stream[2], 0xADu);
		EXPECT_EQ(stream[3], 0xDEu);

#ifdef _WIN64
		EXPECT_EQ(stream.Size(), 8);
		EXPECT_EQ(stream[4], 0x00u);
		EXPECT_EQ(stream[5], 0x00u);
		EXPECT_EQ(stream[6], 0x00u);
		EXPECT_EQ(stream[7], 0x00u);

#else
		EXPECT_EQ(stream.Size(), 4u);
#endif
	}
	{
		ByteStream stream;

		uint8_t bytes[] = { 4, 5, 6 };
		stream << bytes;
		EXPECT_EQ(stream[0], 0x04u);
		EXPECT_EQ(stream[1], 0x05u);
		EXPECT_EQ(stream[2], 0x06u);
		EXPECT_EQ(stream.Size(), 3u);
	}
	{
		ByteStream stream;

		std::vector<uint8_t> data(0x3);
		stream << data;

		EXPECT_EQ(stream[0], 0x00u);
		EXPECT_EQ(stream[1], 0x00u);
		EXPECT_EQ(stream[2], 0x00u);
		EXPECT_EQ(stream.Size(), 3u);
	}
	{
		ByteStream stream;

		std::array<uint8_t, 3> data = { 4, 5, 6};
		stream << data;

		EXPECT_EQ(stream[0], 0x04u);
		EXPECT_EQ(stream[1], 0x05u);
		EXPECT_EQ(stream[2], 0x06u);
		EXPECT_EQ(stream.Size(), 3u);
	}
}

TEST(ByteStreamTests, Fill)
{
	ByteStream stream;
	stream.Add(3, 0x90);
	EXPECT_EQ(stream.Size(), 3u);
	EXPECT_EQ(stream[0], 0x90u);
	EXPECT_EQ(stream[1], 0x90u);
	EXPECT_EQ(stream[2], 0x90u);
	EXPECT_THROW(stream[3], std::out_of_range);

	stream.Add(3, 0x00);
	EXPECT_EQ(stream.Size(), 6u);
	EXPECT_EQ(stream[3], 0x00u);
	EXPECT_EQ(stream[4], 0x00u);
	EXPECT_EQ(stream[5], 0x00u);
	EXPECT_THROW(stream[6], std::out_of_range);
}

TEST(ByteStreamTests, FromString)
{
	{
		ByteStream stream("00 FF 00 FF 00 88");

		EXPECT_EQ(stream.Size(), 6u);
		EXPECT_EQ(stream[0], 0x00u);
		EXPECT_EQ(stream[1], 0xFFu);
		EXPECT_EQ(stream[2], 0x00u);
		EXPECT_EQ(stream[3], 0xFFu);
		EXPECT_EQ(stream[4], 0x00u);
		EXPECT_EQ(stream[5], 0x88u);
	}
	{
		ByteStream stream;
		stream << "FF 00 FF 00 FF 88";

		EXPECT_EQ(stream.Size(), 6u);
		EXPECT_EQ(stream[0], 0xFFu);
		EXPECT_EQ(stream[1], 0x00u);
		EXPECT_EQ(stream[2], 0xFFu);
		EXPECT_EQ(stream[3], 0x00u);
		EXPECT_EQ(stream[4], 0xFFu);
		EXPECT_EQ(stream[5], 0x88u);
	}
}

TEST(ByteStreamTests, FromGoo)
{
	ByteStream stream;
	EXPECT_THROW(stream << "FF00", std::invalid_argument);
	EXPECT_THROW(stream << "FF FFFF", std::invalid_argument);
	EXPECT_THROW(stream << "FF 00FF", std::invalid_argument);
	EXPECT_THROW(stream << "F", std::invalid_argument);
	EXPECT_THROW(stream << "X", std::invalid_argument);
	EXPECT_THROW(stream << "XX", std::invalid_argument);
	EXPECT_THROW(stream << "FF\tFF", std::invalid_argument);
}

TEST(ByteStreamTests, ToString)
{
	{
		uint8_t bytes[] = { 0xA, 0xB, 0xC, 0xFF };
		ByteStream byteStream(bytes);

		std::stringstream stringStream;
		stringStream << byteStream;

		EXPECT_STREQ(stringStream.str().c_str(), "0A 0B 0C FF");

		stringStream << " - " << 1 << 4 << 8 << 16 << 32;

		EXPECT_STREQ(stringStream.str().c_str(), "0A 0B 0C FF - 1481632");
	}
}