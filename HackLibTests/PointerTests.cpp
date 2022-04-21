#include "Pointer.hpp"

TEST(PointerTests, Arithmetic)
{
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 20;

		Pointer ptr2;
		ptr2.Bytes[0] = 30;

		Pointer ptr3 = ptr2 - ptr1;

		EXPECT_EQ(ptr1.Bytes[0], 20u);
		EXPECT_EQ(ptr2.Bytes[0], 30u);
		EXPECT_EQ(ptr3.Bytes[0], 10u);
	}
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 20;

		Pointer ptr2;
		ptr2.Bytes[0] = 30;

		Pointer ptr3 = ptr2 + ptr1;

		EXPECT_EQ(ptr1.Bytes[0], 20u);
		EXPECT_EQ(ptr2.Bytes[0], 30u);
		EXPECT_EQ(ptr3.Bytes[0], 50u);
	}
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 10;

		Pointer ptr2;
		ptr2.Bytes[0] = 10;

		Pointer ptr3 = ptr2 - ptr1;

		EXPECT_EQ(ptr1.Bytes[0], 10u);
		EXPECT_EQ(ptr2.Bytes[0], 10u);
		EXPECT_TRUE(ptr3.Value == nullptr);
	}
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 10;

		ptr1 + 10;

		EXPECT_EQ(ptr1.Bytes[0], 10u);
	}
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 10;

		ptr1 += 10;

		EXPECT_EQ(ptr1.Bytes[0], 20u);
	}
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 10;

		ptr1 - 10;

		EXPECT_EQ(ptr1.Bytes[0], 10u);
	}
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 10;

		ptr1 -= 10;

		EXPECT_EQ(ptr1.Bytes[0], 0u);
	}
	{
		Pointer ptr1;
		std::memset(ptr1.Bytes, 0xFFu, PointerSizeBytes);

		ptr1 -= 0XFF;

		EXPECT_EQ(ptr1.Bytes[0], 0x00u);
	}
	{
		Pointer ptr1;
		std::memset(ptr1.Bytes, 0xFFu, PointerSizeBytes);

		ptr1 += 0XFF;

		EXPECT_EQ(reinterpret_cast<size_t>(ptr1.Value), 0xFEu); // Overflow
	}
}

TEST(PointerTests, Compare)
{
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 20;

		Pointer ptr2;
		ptr2.Bytes[0] = 30;

		ASSERT_GT(ptr2, ptr1);
		ASSERT_LE(ptr1, ptr2);
		ASSERT_NE(ptr1, ptr2);
		ASSERT_NE(ptr2, ptr1);
	}
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 20;

		Pointer ptr2;
		ptr2.Bytes[0] = 20;

		ASSERT_EQ(ptr1, ptr2);
	}
}

TEST(PointerTests, ToString)
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