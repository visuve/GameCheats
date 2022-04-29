#include "MemoryRegion.hpp"

TEST(MemoryRegionTests, Construct)
{
	MemoryRegion region(Pointer(0u),
	{
		{ "foo", typeid(int32_t*) },
		{ "bar", typeid(int32_t*) },
		{ "foobar", typeid(int32_t*) },
		{ "barfoo", typeid(int32_t*) }
	});

#ifdef _WIN64
	EXPECT_EQ(region["foo"], Pointer(0u));
	EXPECT_EQ(region["bar"], Pointer(8u));
	EXPECT_EQ(region["foobar"], Pointer(16u));
	EXPECT_EQ(region["barfoo"], Pointer(24u));
#else
	EXPECT_EQ(region["foo"], Pointer(0u));
	EXPECT_EQ(region["bar"], Pointer(4u));
	EXPECT_EQ(region["foobar"], Pointer(8u));
	EXPECT_EQ(region["barfoo"], Pointer(12u));
#endif
}

TEST(MemoryRegionTests, OutOfRange)
{
	MemoryRegion region(Pointer(0u),
	{
		{ "foo", typeid(int32_t*) },
		{ "bar", typeid(int32_t*) },
		{ "foobar", typeid(int32_t*) },
		{ "barfoo", typeid(int32_t*) }
	});

	EXPECT_THROW(region["barbapapa"], std::out_of_range);
}