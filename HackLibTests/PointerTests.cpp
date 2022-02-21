#include "HackLibTests-PCH.hpp"

TEST(PointerTests, Arithmetic)
{
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 20;

		Pointer ptr2;
		ptr2.Bytes[0] = 30;

		Pointer ptr3 = ptr2 - ptr1;

		EXPECT_EQ(ptr1.Bytes[0], 20);
		EXPECT_EQ(ptr2.Bytes[0], 30);
		EXPECT_EQ(ptr3.Bytes[0], 10);
	}
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 20;

		Pointer ptr2;
		ptr2.Bytes[0] = 30;

		Pointer ptr3 = ptr2 + ptr1;

		EXPECT_EQ(ptr1.Bytes[0], 20);
		EXPECT_EQ(ptr2.Bytes[0], 30);
		EXPECT_EQ(ptr3.Bytes[0], 50);
	}
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 10;

		Pointer ptr2;
		ptr2.Bytes[0] = 10;

		Pointer ptr3 = ptr2 - ptr1;

		EXPECT_EQ(ptr1.Bytes[0], 10);
		EXPECT_EQ(ptr2.Bytes[0], 10);
		EXPECT_TRUE(ptr3.Value == nullptr);
	}
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 10;

		ptr1 + 10;

		EXPECT_EQ(ptr1.Bytes[0], 10);
	}
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 10;

		ptr1 += 10;

		EXPECT_EQ(ptr1.Bytes[0], 20);
	}
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 10;

		ptr1 - 10;

		EXPECT_EQ(ptr1.Bytes[0], 10);
	}
	{
		Pointer ptr1;
		ptr1.Bytes[0] = 10;

		ptr1 -= 10;

		EXPECT_EQ(ptr1.Bytes[0], 0);
	}
	{
		Pointer ptr1;
		std::memset(ptr1.Bytes, 0xFF, PointerSizeBytes);

		ptr1 -= 0XFF;

		EXPECT_EQ(ptr1.Bytes[0], 0x00);
	}
	{
		Pointer ptr1;
		std::memset(ptr1.Bytes, 0xFF, PointerSizeBytes);

		ptr1 += 0XFF;

		EXPECT_EQ(reinterpret_cast<size_t>(ptr1.Value), 0xFE); // Overflow
	}

}