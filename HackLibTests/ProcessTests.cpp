#include "Process.hpp"

#ifndef _WIN64
TEST(ProcessTests, Jump)
{
	{
		auto jump = Process::JumpOp(
			reinterpret_cast<uint8_t*>(0x009A0000),
			reinterpret_cast<uint8_t*>(0x009A0201));

		EXPECT_EQ(jump[0], 0xE9u);
		EXPECT_EQ(jump[1], 0xFCu);
		EXPECT_EQ(jump[2], 0x01u);
		EXPECT_EQ(jump[3], 0x00u);
		EXPECT_EQ(jump[4], 0x00u);
	}
	{
		auto jump = Process::JumpOp(
			reinterpret_cast<uint8_t*>(0x009A0201),
			reinterpret_cast<uint8_t*>(0x009A0000));

		EXPECT_EQ(jump[0], 0xE9u);
		EXPECT_EQ(jump[1], 0xFAu);
		EXPECT_EQ(jump[2], 0xFDu);
		EXPECT_EQ(jump[3], 0xFFu);
		EXPECT_EQ(jump[4], 0xFFu);
	}
}
#endif