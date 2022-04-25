#include "System.hpp"

TEST(SystemTests, Guid)
{
	auto a = System::GenerateGuid();
	auto b = System::GenerateGuid();

	EXPECT_NE(a, b);
	EXPECT_EQ(a.size(), 38);
	EXPECT_EQ(b.size(), 38);
}

TEST(SystemTests, PageSize)
{
	// https://devblogs.microsoft.com/oldnewthing/20210510-00/?p=105200
	EXPECT_GE(System::PageSize(), 0x1000);
}