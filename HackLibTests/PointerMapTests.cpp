#include "PointerMap.hpp"

TEST(PointerMapTests, Construct)
{
	PointerMap ptrMap(Pointer(0u),
	{
		{ "foo", typeid(int) },
		{ "bar", typeid(int) },
		{ "foobar", typeid(int) },
		{ "barfoo", typeid(int) }
	});

#ifdef _WIN64
	EXPECT_EQ(ptrMap["foo"], Pointer(0u));
	EXPECT_EQ(ptrMap["bar"], Pointer(8u));
	EXPECT_EQ(ptrMap["foobar"], Pointer(16u));
	EXPECT_EQ(ptrMap["barfoo"], Pointer(24u));
#else
	EXPECT_EQ(ptrMap["foo"], Pointer(0u));
	EXPECT_EQ(ptrMap["bar"], Pointer(4u));
	EXPECT_EQ(ptrMap["foobar"], Pointer(8u));
	EXPECT_EQ(ptrMap["barfoo"], Pointer(12u));
#endif
}

TEST(PointerMapTests, OutOfRange)
{
	PointerMap ptrMap(Pointer(0u), 
	{
		{ "foo", typeid(int) },
		{ "bar", typeid(int) },
		{ "foobar", typeid(int) },
		{ "barfoo", typeid(int) }
	});

	EXPECT_THROW(ptrMap["barbapapa"], std::out_of_range);
}