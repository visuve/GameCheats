#include "HackLibTests.pch"

TEST(StrConvert, ToUtf8)
{
	{
		std::string test = StrConvert::ToUtf8(L"foobar");
		EXPECT_STREQ(test.c_str(), "foobar");
		EXPECT_EQ(test.size(), 6);
	}
	{
		std::string test = StrConvert::ToUtf8(L"");
		EXPECT_STREQ(test.c_str(), "");
		EXPECT_EQ(test.size(),0);
	}
	{
		std::wstring test = StrConvert::ToUtf8("foobar");
		EXPECT_STREQ(test.c_str(), L"foobar");
		EXPECT_EQ(test.size(), 6);
	}
	{
		std::wstring test = StrConvert::ToUtf8("");
		EXPECT_STREQ(test.c_str(), L"");
		EXPECT_EQ(test.size(), 0);
	}
}