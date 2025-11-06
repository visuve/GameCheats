#include "Wildcard.hpp"

TEST(WildCardTests, ByteWildcardCtor)
{
	EXPECT_NO_THROW(ByteWildcard("00 FF 00 FF 00 88"));
	EXPECT_NO_THROW(ByteWildcard("00 FF 00 FF 0? 88"));
	EXPECT_NO_THROW(ByteWildcard("00 FF 00 FF ?? 88"));
	EXPECT_NO_THROW(ByteWildcard("00 FF 00 FF ?0 88"));
	EXPECT_NO_THROW(ByteWildcard("00 FF 00 FF 0* 88"));
	EXPECT_NO_THROW(ByteWildcard("00 FF 00 FF ** 88"));
	EXPECT_NO_THROW(ByteWildcard("00 FF 00 FF *0 88"));

	EXPECT_THROW(ByteWildcard("00 FF 00 FF 00 88 p"), std::invalid_argument);
	EXPECT_THROW(ByteWildcard("00 FF 00 FF * 88"), std::invalid_argument);
}

TEST(WildcardTests, Matches)
{
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF 0? 88").Matches(ByteStream("00 FF 00 FF 0A 88")));
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF ?? 88").Matches(ByteStream("00 FF 00 FF AA 88")));
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF ?0 88").Matches(ByteStream("00 FF 00 FF A0 88")));

	EXPECT_TRUE(ByteWildcard("00 FF 00 FF *0 88").Matches(ByteStream("00 FF 00 FF AA BB C0 88")));
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF ** 88").Matches(ByteStream("00 FF 00 FF AA BB CC 88")));
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF 0* 88").Matches(ByteStream("00 FF 00 FF 0A BB CC 88")));
}

TEST(WildcardTests, NoMatch)
{
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF ?0 88").Matches(ByteStream("00 FF 00 FF AA BB C0 88")));
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF ?0 88").Matches(ByteStream("00 FF 00 FF 88")));
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF ?? 88").Matches(ByteStream("00 FF 00 FF AA BB CC 88")));
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF ?? 88").Matches(ByteStream("00 FF 00 FF 88")));
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF 0? 88").Matches(ByteStream("00 FF 00 FF 0A BB CC 88")));
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF 0? 88").Matches(ByteStream("00 FF 00 FF 88")));

	EXPECT_FALSE(ByteWildcard("00 FF 00 FF 0* 88").Matches(ByteStream("00 FF 00 FF AA 88")));
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF 0* 88").Matches(ByteStream("00 FF 00 FF 88")));
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF ** 88").Matches(ByteStream("00 FF 00 FF 88")));
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF *0 88").Matches(ByteStream("00 FF 00 FF 88")));
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF *0 88").Matches(ByteStream("00 FF 00 FF AA 88")));
}