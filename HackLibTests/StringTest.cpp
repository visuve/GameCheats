#include "Strings.hpp"

TEST(Strings, ToUtf8)
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

TEST(Strings, IEquals)
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

TEST(Strings, IsHex)
{
	EXPECT_TRUE(Strings::IsHex('0'));
	EXPECT_TRUE(Strings::IsHex('9'));
	EXPECT_TRUE(Strings::IsHex('A'));
	EXPECT_FALSE(Strings::IsHex('a'));
	EXPECT_TRUE(Strings::IsHex('F'));
	EXPECT_FALSE(Strings::IsHex('f'));

	EXPECT_FALSE(Strings::IsHex('G'));
	EXPECT_FALSE(Strings::IsHex('g'));
	EXPECT_FALSE(Strings::IsHex('/'));
	EXPECT_FALSE(Strings::IsHex(':'));
	EXPECT_FALSE(Strings::IsHex('@'));
	EXPECT_FALSE(Strings::IsHex('['));
	EXPECT_FALSE(Strings::IsHex('`'));
	EXPECT_FALSE(Strings::IsHex('{'));
	EXPECT_FALSE(Strings::IsHex('?'));
}

TEST(Strings, ValueFromNibble)
{
	EXPECT_EQ(Strings::ValueFromNibble('0'), 0x0);
	EXPECT_EQ(Strings::ValueFromNibble('9'), 0x9);
	EXPECT_EQ(Strings::ValueFromNibble('A'), 0xA);
	EXPECT_THROW(Strings::ValueFromNibble('a'), std::invalid_argument);
	EXPECT_EQ(Strings::ValueFromNibble('F'), 0xF);
	EXPECT_THROW(Strings::ValueFromNibble('f'), std::invalid_argument);

	EXPECT_THROW(Strings::ValueFromNibble('G'), std::invalid_argument);
	EXPECT_THROW(Strings::ValueFromNibble('?'), std::invalid_argument);
}

TEST(Strings, NibbleFromValue)
{
	EXPECT_EQ(Strings::NibbleFromValue(0x0), '0');
	EXPECT_EQ(Strings::NibbleFromValue(0x9), '9');
	EXPECT_EQ(Strings::NibbleFromValue(0xA), 'A');
	EXPECT_EQ(Strings::NibbleFromValue(0xF), 'F');
}

TEST(Strings, ValueFromNibbles)
{
	EXPECT_EQ(Strings::ValueFromNibbles({ '0', '0' }), 0x00);
	EXPECT_EQ(Strings::ValueFromNibbles({ '9', '9' }), 0x99);
	EXPECT_EQ(Strings::ValueFromNibbles({ '0', 'F' }), 0x0F);
	EXPECT_EQ(Strings::ValueFromNibbles({ 'F', '0' }), 0xF0);
	EXPECT_EQ(Strings::ValueFromNibbles({ 'F', 'F' }), 0xFF);

	EXPECT_THROW(Strings::ValueFromNibbles({ '0', 'G' }), std::invalid_argument);
	EXPECT_THROW(Strings::ValueFromNibbles({ 'G', '0' }), std::invalid_argument);
}

TEST(Strings, NibblesFromValue)
{
	{
		auto [high, low] = Strings::NibblesFromValue(0x00);
		EXPECT_EQ(high, '0');
		EXPECT_EQ(low, '0');
	}
	{
		auto [high, low] = Strings::NibblesFromValue(0x99);
		EXPECT_EQ(high, '9');
		EXPECT_EQ(low, '9');
	}
	{
		auto [high, low] = Strings::NibblesFromValue(0x0F);
		EXPECT_EQ(high, '0');
		EXPECT_EQ(low, 'F');
	}
	{
		auto [high, low] = Strings::NibblesFromValue(0xF0);
		EXPECT_EQ(high, 'F');
		EXPECT_EQ(low, '0');
	}
	{
		auto [high, low] = Strings::NibblesFromValue(0xFF);
		EXPECT_EQ(high, 'F');
		EXPECT_EQ(low, 'F');
	}
}