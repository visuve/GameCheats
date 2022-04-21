#include "HackLibTests.pch"

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