#include "Wildcard.hpp"

TEST(WildCardTests, WildcardSimple)
{
	EXPECT_FALSE(Wildcard("ABC").Matches("ABCDEF").empty());
	EXPECT_FALSE(Wildcard("CDE").Matches("ABCDEF").empty());
	EXPECT_FALSE(Wildcard("DEF").Matches("ABCDEF").empty());
	EXPECT_FALSE(Wildcard("*").Matches("ABCDEF").empty());
	EXPECT_FALSE(Wildcard("?").Matches("ABCDEF").empty());
	EXPECT_FALSE(Wildcard("A?").Matches("ABCDEF").empty());
	EXPECT_FALSE(Wildcard("B??E").Matches("ABCDEF").empty());
	EXPECT_FALSE(Wildcard("?F").Matches("ABCDEF").empty());
	EXPECT_FALSE(Wildcard("A*").Matches("ABCDEF").empty());
	EXPECT_FALSE(Wildcard("B*E").Matches("ABCDEF").empty());
	EXPECT_FALSE(Wildcard("*F").Matches("ABCDEF").empty());
}

TEST(WildcardTests, WildcardNoMatch)
{
	EXPECT_TRUE(Wildcard("XYZ").Matches("ABCDEF").empty());
	EXPECT_TRUE(Wildcard("A?D").Matches("ABCDEF").empty());
	EXPECT_TRUE(Wildcard("A*G").Matches("ABCDEF").empty());
	EXPECT_TRUE(Wildcard("?A").Matches("ABCDEF").empty());
	EXPECT_FALSE(Wildcard("*A").Matches("ABCDEF").empty()); // * is zero or more
}

TEST(WildCardTests, WildcardEmpty)
{
	EXPECT_TRUE(Wildcard("").Matches("ABCDEF").empty());
	EXPECT_TRUE(Wildcard("ABC").Matches("").empty());
	EXPECT_TRUE(Wildcard("").Matches("").empty());
}

TEST(WildcardTests, WildcardIndexes)
{
	{
		auto result = Wildcard("ABC").Matches("ABCDEF");
		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0], 0);

	}
	{
		auto result = Wildcard("CDE").Matches("ABCDEF");
		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0], 2);
	}
	{
		auto result = Wildcard("DEF").Matches("ABCDEF");
		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0], 3);
		
	}
	{
		auto result = Wildcard("A?").Matches("ABCDEF");
		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0], 0);
	}
	{
		auto result = Wildcard("B??E").Matches("ABCDEF");
		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0], 1);
	}
	{
		auto result = Wildcard("?F").Matches("ABCDEF");
		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0], 4);
	
	}
	{
		auto result = Wildcard("A*").Matches("ABCDEF");
		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0], 0);

	}
	{
		auto result = Wildcard("B*E").Matches("ABCDEF");
		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0], 1);
	}
	{
		auto result = Wildcard("*F").Matches("ABCDEF");
		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0], 0);
	}
}

TEST(WildCardTests, ByteWildcardEmpty)
{
	EXPECT_TRUE(ByteWildcard("").Matches(ByteStream("AA BB CC")).empty());
	EXPECT_TRUE(ByteWildcard("AA BB").Matches(ByteStream("")).empty());
	EXPECT_TRUE(ByteWildcard("").Matches(ByteStream("")).empty());
}

TEST(WildcardTests, ByteWildcardCtor)
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
	EXPECT_THROW(ByteWildcard("GG"), std::invalid_argument);
}

TEST(WildcardTests, ByteWildcardMatches)
{
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF 0? 88").Matches(ByteStream("00 FF 00 FF 0A 88")).empty());
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF ?? 88").Matches(ByteStream("00 FF 00 FF AA 88")).empty());
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF ?0 88").Matches(ByteStream("00 FF 00 FF A0 88")).empty());

	EXPECT_FALSE(ByteWildcard("00 FF 00 FF *0 88").Matches(ByteStream("00 FF 00 FF AA BB C0 88")).empty());
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF ** 88").Matches(ByteStream("00 FF 00 FF AA BB CC 88")).empty());
	EXPECT_FALSE(ByteWildcard("00 FF 00 FF 0* 88").Matches(ByteStream("00 FF 00 FF 0A BB CC 88")).empty());
}

TEST(WildcardTests, ByteWildcardNoMatch)
{
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF ?0 88").Matches(ByteStream("00 FF 00 FF AA BB C0 88")).empty());
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF ?0 88").Matches(ByteStream("00 FF 00 FF 88")).empty());
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF ?? 88").Matches(ByteStream("00 FF 00 FF AA BB CC 88")).empty());
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF ?? 88").Matches(ByteStream("00 FF 00 FF 88")).empty());
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF 0? 88").Matches(ByteStream("00 FF 00 FF 0A BB CC 88")).empty());
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF 0? 88").Matches(ByteStream("00 FF 00 FF 88")).empty());

	EXPECT_TRUE(ByteWildcard("00 FF 00 FF 0* 88").Matches(ByteStream("00 FF 00 FF AA 88")).empty());
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF 0* 88").Matches(ByteStream("00 FF 00 FF 88")).empty());
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF ** 88").Matches(ByteStream("00 FF 00 FF 88")).empty());
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF *0 88").Matches(ByteStream("00 FF 00 FF 88")).empty());
	EXPECT_TRUE(ByteWildcard("00 FF 00 FF *0 88").Matches(ByteStream("00 FF 00 FF AA 88")).empty());
}

TEST(WildcardTests, ByteWildcardNibbles)
{
	EXPECT_FALSE(ByteWildcard("?A BB").Matches(ByteStream("FA BB")).empty());
	EXPECT_FALSE(ByteWildcard("?A BB").Matches(ByteStream("0A BB")).empty());

	EXPECT_FALSE(ByteWildcard("A? BB").Matches(ByteStream("AF BB")).empty());
	EXPECT_FALSE(ByteWildcard("A? BB").Matches(ByteStream("A0 BB")).empty());

	EXPECT_FALSE(ByteWildcard("?? BB").Matches(ByteStream("FF BB")).empty());
	EXPECT_FALSE(ByteWildcard("?? BB").Matches(ByteStream("00 BB")).empty());

	// Adjusted for scanning behavior: Pattern will be found at index 0
	EXPECT_FALSE(ByteWildcard("?? BB").Matches(ByteStream("FF BB CC")).empty());
}

TEST(WildcardTests, ByteWildcardSequences)
{
	EXPECT_FALSE(ByteWildcard("11 *3 44").Matches(ByteStream("11 AA BB C3 44")).empty());
	EXPECT_FALSE(ByteWildcard("** 99").Matches(ByteStream("11 22 33 99")).empty());
	EXPECT_FALSE(ByteWildcard("11 0*").Matches(ByteStream("11 0A BB CC")).empty());
	EXPECT_FALSE(ByteWildcard("11 ** 55 ** 99").Matches(ByteStream("11 22 33 55 66 77 99")).empty());
}

TEST(WildcardTests, ByteWildcardIndexes)
{
	{
		auto result = ByteWildcard("AA BB").Matches(ByteStream("AA BB CC"));
		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0], 0);
	}
	{
		auto result = ByteWildcard("BB CC").Matches(ByteStream("AA BB CC"));
		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0], 1);
	}
	{
		auto result = ByteWildcard("AA BB").Matches(ByteStream("00 11 AA BB 22 33"));
		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0], 2);
	}

	{
		auto result = ByteWildcard("FF ??").Matches(ByteStream("FF 00 11 FF AA 22 FF BB"));
		ASSERT_EQ(result.size(), 3);
		EXPECT_EQ(result[0], 0);
		EXPECT_EQ(result[1], 3);
		EXPECT_EQ(result[2], 6);
	}

	{
		auto result = ByteWildcard("11 ** 99").Matches(ByteStream("00 11 22 33 99 00"));
		ASSERT_EQ(result.size(), 1);
		EXPECT_EQ(result[0], 1);
	}
}

TEST(WildcardTests, ByteWildcardOverlappingMatches)
{
	// Finding "AA ** AA" in "AA 00 AA 11 AA" 
	// The scanner iterates startIdx, so it will find matches starting at both indexes where AA is present
	auto result = ByteWildcard("AA ** AA").Matches(ByteStream("AA 00 AA 11 AA"));
	ASSERT_EQ(result.size(), 2);
	EXPECT_EQ(result[0], 0); // Matches entire stream: AA -> 00 AA 11 -> AA
	EXPECT_EQ(result[1], 2); // Matches sub stream: AA -> 11 -> AA
}

TEST(WildcardTests, ByteWildcardPatternLongerThanStream)
{
	EXPECT_TRUE(ByteWildcard("AA BB CC").Matches(ByteStream("AA BB")).empty());
}

TEST(WildcardTests, ByteWildcardGreedyBacktracking)
{
	// Forces '*' to over-consume, then backtrack
	EXPECT_FALSE(ByteWildcard("AA ** BB CC").Matches(ByteStream("AA 11 22 BB CC")).empty());
}

TEST(WildcardTests, ByteWildcardMultipleBacktrackingSteps)
{
	EXPECT_FALSE(ByteWildcard("AA ** BB ** CC").Matches(ByteStream("AA 11 22 BB 33 44 CC")).empty());
}

TEST(WildcardTests, ByteWildcardSingleByteMatch)
{
	EXPECT_FALSE(ByteWildcard("AA ** BB").Matches(ByteStream("AA 11 BB")).empty());
}

TEST(WildcardTests, ByteWildcardConsumesEntireMiddle)
{
	EXPECT_FALSE(ByteWildcard("AA ** BB").Matches(ByteStream("AA 11 22 33 44 BB")).empty());
}

TEST(WildcardTests, ByteWildcardMixedNibbleAndSequence)
{
	EXPECT_THROW(ByteWildcard("00 FF 00 FF ?* 88"), std::invalid_argument);
	EXPECT_THROW(ByteWildcard("00 FF 00 FF *? 88"), std::invalid_argument);
}

TEST(WildcardTests, ByteWildcardFullWildcardPattern)
{
	auto res = ByteWildcard("**").Matches(ByteStream("AA BB CC"));
	EXPECT_FALSE(res.empty()); // should match at multiple positions
}

TEST(WildcardTests, ByteWildcardDeterministicResults)
{
	ByteWildcard wc("AA ** BB");
	ByteStream stream("AA 11 22 BB");

	auto r1 = wc.Matches(stream);
	auto r2 = wc.Matches(stream);

	EXPECT_EQ(r1, r2);
}