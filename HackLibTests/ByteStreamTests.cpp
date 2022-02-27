#include "HackLibTests-PCH.hpp"

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