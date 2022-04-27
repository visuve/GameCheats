#include "Pointer.hpp"

TEST(PointerTests, Access)
{
	Pointer ptr;
	std::fill(ptr.begin(), ptr.end(), uint8_t(0x88));

	EXPECT_EQ(ptr[0], 0x88u);
	EXPECT_EQ(ptr[1], 0x88u);
	EXPECT_EQ(ptr[2], 0x88u);
	EXPECT_EQ(ptr[3], 0x88u);
#ifdef _WIN64
	EXPECT_EQ(ptr[4], 0x88u);
	EXPECT_EQ(ptr[5], 0x88u);
	EXPECT_EQ(ptr[6], 0x88u);
	EXPECT_EQ(ptr[7], 0x88u);
	EXPECT_THROW(ptr[8], std::out_of_range);
#else
	EXPECT_THROW(ptr[4], std::out_of_range);
#endif

}

TEST(PointerTests, Arithmetic)
{
	{
		Pointer ptr1;
		ptr1[0] = 20;

		Pointer ptr2;
		ptr2[0] = 30;

		Pointer ptr3 = ptr2 - ptr1;

		EXPECT_EQ(ptr1[0], 20u);
		EXPECT_EQ(ptr2[0], 30u);
		EXPECT_EQ(ptr3[0], 10u);
	}
	{
		Pointer ptr1;
		ptr1[0] = 20;

		Pointer ptr2;
		ptr2[0] = 30;

		Pointer ptr3 = ptr2 + ptr1;

		EXPECT_EQ(ptr1[0], 20u);
		EXPECT_EQ(ptr2[0], 30u);
		EXPECT_EQ(ptr3[0], 50u);
	}
	{
		Pointer ptr1;
		ptr1[0] = 10;

		Pointer ptr2;
		ptr2[0] = 10;

		Pointer ptr3 = ptr2 - ptr1;

		EXPECT_EQ(ptr1[0], 10u);
		EXPECT_EQ(ptr2[0], 10u);
		EXPECT_TRUE(ptr3.Value() == nullptr);
	}
	{
		Pointer ptr1;
		ptr1[0] = 10;

		ptr1 + 10;

		EXPECT_EQ(ptr1[0], 10u);
	}
	{
		Pointer ptr1;
		ptr1[0] = 10;

		ptr1 += 10;

		EXPECT_EQ(ptr1[0], 20u);
	}
	{
		Pointer ptr1;
		ptr1[0] = 10;

		ptr1 - 10;

		EXPECT_EQ(ptr1[0], 10u);
	}
	{
		Pointer ptr1;
		ptr1[0] = 10;

		ptr1 -= 10;

		EXPECT_EQ(ptr1[0], 0u);
	}
	{
		Pointer ptr1;
		std::fill(ptr1.begin(), ptr1.end(), uint8_t(0xFF));

		ptr1 -= 0XFF;

		EXPECT_EQ(ptr1[0], 0x00u);
	}
	{
		Pointer ptr1;
		std::fill(ptr1.begin(), ptr1.end(), uint8_t(0xFF));

		ptr1 += 0XFF;

		EXPECT_EQ(reinterpret_cast<size_t>(ptr1.Value()), 0xFEu); // Overflow
	}
	{
		Pointer ptr1;
		++ptr1;

		EXPECT_EQ(ptr1[0], Pointer::Size);

		auto expected = reinterpret_cast<uint8_t*>(Pointer::Size);
		EXPECT_EQ(ptr1.Value(), expected);
	}
	{
		Pointer ptr1;
		--ptr1;

		auto expected = reinterpret_cast<uint8_t*>(0u - Pointer::Size);
		EXPECT_EQ(ptr1.Value(), expected);
	}
	{
		Pointer ptr1;
		++ptr1;
		--ptr1;

		auto expected = reinterpret_cast<uint8_t*>(0u);
		EXPECT_EQ(ptr1.Value(), expected);
	}
	{
		Pointer ptr1;
		ptr1 -= 1u;

		size_t max = std::numeric_limits<size_t>::max();
		auto expected = reinterpret_cast<uint8_t*>(max);
		EXPECT_EQ(ptr1.Value(), expected);
	}
}

TEST(PointerTests, Compare)
{
	{
		Pointer ptr1;
		ptr1[0] = 20;

		Pointer ptr2;
		ptr2[0] = 30;

		ASSERT_GT(ptr2, ptr1);
		ASSERT_LE(ptr1, ptr2);
		ASSERT_NE(ptr1, ptr2);
		ASSERT_NE(ptr2, ptr1);
	}
	{
		Pointer ptr1;
		ptr1[0] = 20;

		Pointer ptr2;
		ptr2[0] = 20;

		ASSERT_EQ(ptr1, ptr2);
	}
}

TEST(PointerTests, ToString)
{
	{
		std::string str = std::format("{0}", Pointer());

#ifdef _WIN64
		EXPECT_STREQ(str.c_str(), "0x0000000000000000");
#else
		EXPECT_STREQ(str.c_str(), "0x00000000");
#endif
	}

	{
		std::string str = std::format("{0}", Pointer(0xDEADBEEFu));

#ifdef _WIN64
		EXPECT_STREQ(str.c_str(), "0x00000000DEADBEEF");
#else
		EXPECT_STREQ(str.c_str(), "0xDEADBEEF");
#endif
	}
	{
		std::string str = std::format("{0}", Pointer(reinterpret_cast<uint8_t*>(-1)));

#ifdef _WIN64
		EXPECT_STREQ(str.c_str(), "0xFFFFFFFFFFFFFFFF");
#else
		EXPECT_STREQ(str.c_str(), "0xFFFFFFFF");
#endif
	}
}

TEST(PointerTests, ToStream)
{
	{
		std::stringstream stream;
		stream << Pointer();

#ifdef _WIN64
		EXPECT_STREQ(stream.str().c_str(), "0x0000000000000000");
#else
		EXPECT_STREQ(stream.str().c_str(), "0x00000000");
#endif
	}

	{
		std::stringstream stream;

#pragma warning(suppress: 4312)
		stream << Pointer(reinterpret_cast<uint8_t*>(0xDEADBEEF));

#ifdef _WIN64
		EXPECT_STREQ(stream.str().c_str(), "0x00000000DEADBEEF");
#else
		EXPECT_STREQ(stream.str().c_str(), "0xDEADBEEF");
#endif
	}
	{
		std::stringstream stream;
		stream << Pointer(reinterpret_cast<uint8_t*>(-1));

#ifdef _WIN64
		EXPECT_STREQ(stream.str().c_str(), "0xFFFFFFFFFFFFFFFF");
#else
		EXPECT_STREQ(stream.str().c_str(), "0xFFFFFFFF");
#endif
	}
}