#include "HackLibTests-PCH.hpp"

TEST(ByteStreamTests, Constructors)
{
	{
		ByteStream stream(3, 0x90);
		EXPECT_EQ(stream[0], 0x90);
		EXPECT_EQ(stream[1], 0x90);
		EXPECT_EQ(stream[2], 0x90);
		EXPECT_EQ(stream.Size(), 3);
	}
	{
		uint8_t bytes[] = { 4, 5, 6 };
		ByteStream stream(bytes);
		EXPECT_EQ(stream[0], 0x04);
		EXPECT_EQ(stream[1], 0x05);
		EXPECT_EQ(stream[2], 0x06);
		EXPECT_EQ(stream.Size(), 3);
	}
	{
		std::vector<uint8_t> data = { 4, 5, 6 };
		ByteStream stream(data);

		EXPECT_EQ(stream[0], 0x04);
		EXPECT_EQ(stream[1], 0x05);
		EXPECT_EQ(stream[2], 0x06);
		EXPECT_EQ(stream.Size(), 3);
	}
	{
		std::array<uint8_t, 3> data = { 4, 5, 6 };
		ByteStream stream(data);

		EXPECT_EQ(stream[0], 0x04);
		EXPECT_EQ(stream[1], 0x05);
		EXPECT_EQ(stream[2], 0x06);
		EXPECT_EQ(stream.Size(), 3);
	}
	{
		ByteStream stream({ 4, 5, 6 });
		EXPECT_EQ(stream[0], 0x04);
		EXPECT_EQ(stream[1], 0x05);
		EXPECT_EQ(stream[2], 0x06);
		EXPECT_EQ(stream.Size(), 3);
	}
}

TEST(ByteStreamTests, StreamOperators)
{
	{
		ByteStream stream;
		stream << Pointer(reinterpret_cast<uint8_t*>(0xDEADBEEF));

		EXPECT_EQ(stream[0], 0xEF);
		EXPECT_EQ(stream[1], 0xBE);
		EXPECT_EQ(stream[2], 0xAD);
		EXPECT_EQ(stream[3], 0xDE);

#ifdef _WIN64
		EXPECT_EQ(stream.Size(), 8);
		EXPECT_EQ(stream[4], 0x00);
		EXPECT_EQ(stream[5], 0x00);
		EXPECT_EQ(stream[6], 0x00);
		EXPECT_EQ(stream[7], 0x00);

#else
		EXPECT_EQ(stream.Size(), 4);
#endif
	}
	{
		ByteStream stream;

		uint8_t bytes[] = { 4, 5, 6 };
		stream << bytes;
		EXPECT_EQ(stream[0], 0x04);
		EXPECT_EQ(stream[1], 0x05);
		EXPECT_EQ(stream[2], 0x06);
		EXPECT_EQ(stream.Size(), 3);
	}
	{
		ByteStream stream;

		std::vector<uint8_t> data(0x3);
		stream << data;

		EXPECT_EQ(stream[0], 0x00);
		EXPECT_EQ(stream[1], 0x00);
		EXPECT_EQ(stream[2], 0x00);
		EXPECT_EQ(stream.Size(), 3);
	}
	{
		ByteStream stream;

		std::array<uint8_t, 3> data = { 4, 5, 6};
		stream << data;

		EXPECT_EQ(stream[0], 0x04);
		EXPECT_EQ(stream[1], 0x05);
		EXPECT_EQ(stream[2], 0x06);
		EXPECT_EQ(stream.Size(), 3);
	}
}

TEST(ByteStreamTests, Fill)
{
	ByteStream stream;
	stream.Fill(3, 0x90);
	EXPECT_EQ(stream.Size(), 3);
	EXPECT_EQ(stream[0], 0x90);
	EXPECT_EQ(stream[1], 0x90);
	EXPECT_EQ(stream[2], 0x90);

	stream.Fill(3, 0x00);
	EXPECT_EQ(stream.Size(), 6);
	EXPECT_EQ(stream[3], 0x00);
	EXPECT_EQ(stream[4], 0x00);
	EXPECT_EQ(stream[5], 0x00);
}

TEST(ByteStreamTests, FromString)
{
	{
		ByteStream stream("00 FF 00 FF 00 88");

		EXPECT_EQ(stream.Size(), 6);
		EXPECT_EQ(stream[0], 0x00);
		EXPECT_EQ(stream[1], 0xFF);
		EXPECT_EQ(stream[2], 0x00);
		EXPECT_EQ(stream[3], 0xFF);
		EXPECT_EQ(stream[4], 0x00);
		EXPECT_EQ(stream[5], 0x88);
	}
	{
		ByteStream stream;
		stream << "FF 00 FF 00 FF 88";

		EXPECT_EQ(stream.Size(), 6);
		EXPECT_EQ(stream[0], 0xFF);
		EXPECT_EQ(stream[1], 0x00);
		EXPECT_EQ(stream[2], 0xFF);
		EXPECT_EQ(stream[3], 0x00);
		EXPECT_EQ(stream[4], 0xFF);
		EXPECT_EQ(stream[5], 0x88);
	}
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