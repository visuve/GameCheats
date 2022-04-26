#include "PointerMap.hpp"

TEST(PointerMapTests, Construct)
{
	PointerMap ptrMap(Pointer(), { "foo", "bar", "foobar", "barfoo" });

#ifdef _WIN64
	EXPECT_EQ(ptrMap["foo"].Value(), reinterpret_cast<uint8_t*>(0));
	EXPECT_EQ(ptrMap["bar"].Value(), reinterpret_cast<uint8_t*>(8));
	EXPECT_EQ(ptrMap["foobar"].Value(), reinterpret_cast<uint8_t*>(16));
	EXPECT_EQ(ptrMap["barfoo"].Value(), reinterpret_cast<uint8_t*>(24));
#else
	EXPECT_EQ(ptrMap["foo"].Value(), reinterpret_cast<uint8_t*>(0));
	EXPECT_EQ(ptrMap["bar"].Value(), reinterpret_cast<uint8_t*>(4));
	EXPECT_EQ(ptrMap["foobar"].Value(), reinterpret_cast<uint8_t*>(8));
	EXPECT_EQ(ptrMap["barfoo"].Value(), reinterpret_cast<uint8_t*>(12));
#endif
}

TEST(PointerMapTests, OutOfRange)
{
	PointerMap ptrMap(Pointer(), { "foo", "bar", "foobar", "barfoo" });
	EXPECT_THROW(ptrMap["barbapapa"], std::out_of_range);
}