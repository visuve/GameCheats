#include "StrConvert.hpp"

TEST(StrConvert, ToUtf8)
{
	{
		std::string test = StrConvert::ToUtf8(L"foobar");
		EXPECT_STREQ(test.c_str(), "foobar");
		EXPECT_EQ(test.size(), 6u);
	}
	{
		std::string test = StrConvert::ToUtf8(L"");
		EXPECT_STREQ(test.c_str(), "");
		EXPECT_EQ(test.size(),0u);
	}
	{
		std::wstring test = StrConvert::ToUtf8("foobar");
		EXPECT_STREQ(test.c_str(), L"foobar");
		EXPECT_EQ(test.size(), 6u);
	}
	{
		std::wstring test = StrConvert::ToUtf8("");
		EXPECT_STREQ(test.c_str(), L"");
		EXPECT_EQ(test.size(), 0u);
	}
}

TEST(StrConvert, IEquals)
{
	EXPECT_FALSE(StrConvert::IEquals("foobar", "fooba"));
	EXPECT_FALSE(StrConvert::IEquals("fooba", "foobar"));
	EXPECT_FALSE(StrConvert::IEquals("foobar", "barfoo"));

	EXPECT_TRUE(StrConvert::IEquals("foobar", "fooBAR"));
	EXPECT_TRUE(StrConvert::IEquals("foobar", "FOObar"));
	EXPECT_TRUE(StrConvert::IEquals("fooBAR", "foobar"));
	EXPECT_TRUE(StrConvert::IEquals("FOObar", "foobar"));
}