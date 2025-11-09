#include "Strings.hpp"

TEST(StrConvert, ToUtf8)
{
	{
		std::string test = Strings::ToUtf8(L"foobar");
		EXPECT_STREQ(test.c_str(), "foobar");
		EXPECT_EQ(test.size(), 6u);
	}
	{
		std::string test = Strings::ToUtf8(L"");
		EXPECT_STREQ(test.c_str(), "");
		EXPECT_EQ(test.size(),0u);
	}
	{
		std::wstring test = Strings::ToUtf8("foobar");
		EXPECT_STREQ(test.c_str(), L"foobar");
		EXPECT_EQ(test.size(), 6u);
	}
	{
		std::wstring test = Strings::ToUtf8("");
		EXPECT_STREQ(test.c_str(), L"");
		EXPECT_EQ(test.size(), 0u);
	}
}

TEST(StrConvert, IEquals)
{
	{
		EXPECT_FALSE(Strings::IEquals("foobar", "fooba"));
		EXPECT_FALSE(Strings::IEquals("fooba", "foobar"));
		EXPECT_FALSE(Strings::IEquals("foobar", "barfoo"));

		EXPECT_TRUE(Strings::IEquals("foobar", "fooBAR"));
		EXPECT_TRUE(Strings::IEquals("foobar", "FOObar"));
		EXPECT_TRUE(Strings::IEquals("fooBAR", "foobar"));
		EXPECT_TRUE(Strings::IEquals("FOObar", "foobar"));
	}
	{
		EXPECT_TRUE(Strings::IEquals('a', 'A'));
		EXPECT_TRUE(Strings::IEquals('A', 'a'));
		EXPECT_FALSE(Strings::IEquals('A', 'B'));
	}
}

TEST(StrConvert, IsAlphaNum)
{
	EXPECT_TRUE(Strings::IsAlphaNumeric('A'));
	EXPECT_TRUE(Strings::IsAlphaNumeric('a'));
	EXPECT_FALSE(Strings::IsAlphaNumeric('G'));
	EXPECT_FALSE(Strings::IsAlphaNumeric('g'));
}