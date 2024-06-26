#include "TypeHelp.hpp"

TEST(TypeHelpTests, SizeOfBasicType)
{
	EXPECT_EQ(SizeOfBasicType(typeid(Pointer)), Pointer::Size);
	EXPECT_EQ(SizeOfBasicType(typeid(uint8_t)), 1);
	EXPECT_EQ(SizeOfBasicType(typeid(uint16_t)), 2);
	EXPECT_EQ(SizeOfBasicType(typeid(uint32_t)), 4);
	EXPECT_EQ(SizeOfBasicType(typeid(uint64_t)), 8);
	EXPECT_EQ(SizeOfBasicType(typeid(int8_t)), 1);
	EXPECT_EQ(SizeOfBasicType(typeid(int16_t)), 2);
	EXPECT_EQ(SizeOfBasicType(typeid(int32_t)), 4);
	EXPECT_EQ(SizeOfBasicType(typeid(int64_t)), 8);
	EXPECT_EQ(SizeOfBasicType(typeid(float)), 4);
	EXPECT_EQ(SizeOfBasicType(typeid(double)), 8);
	EXPECT_EQ(SizeOfBasicType(typeid(uint8_t*)), Pointer::Size);
	EXPECT_EQ(SizeOfBasicType(typeid(uint16_t*)), Pointer::Size);
	EXPECT_EQ(SizeOfBasicType(typeid(uint32_t*)), Pointer::Size);
	EXPECT_EQ(SizeOfBasicType(typeid(uint64_t*)), Pointer::Size);
	EXPECT_EQ(SizeOfBasicType(typeid(int8_t*)), Pointer::Size);
	EXPECT_EQ(SizeOfBasicType(typeid(int16_t*)), Pointer::Size);
	EXPECT_EQ(SizeOfBasicType(typeid(int64_t*)), Pointer::Size);
	EXPECT_EQ(SizeOfBasicType(typeid(float*)), Pointer::Size);
	EXPECT_EQ(SizeOfBasicType(typeid(double*)), Pointer::Size);
	EXPECT_THROW(SizeOfBasicType(typeid(HANDLE)), std::invalid_argument);
}

TEST(TypeHelpTests, ToString)
{
	EXPECT_STREQ(BasicTypeToString(typeid(Pointer)).data(), "\"Pointer\"");
	EXPECT_STREQ(BasicTypeToString(typeid(uint8_t)).data(), "uint8_t");
	EXPECT_STREQ(BasicTypeToString(typeid(uint16_t)).data(), "uint16_t");
	EXPECT_STREQ(BasicTypeToString(typeid(uint32_t)).data(), "uint32_t");
	EXPECT_STREQ(BasicTypeToString(typeid(uint64_t)).data(), "uint64_t");
	EXPECT_STREQ(BasicTypeToString(typeid(int8_t)).data(), "int8_t");
	EXPECT_STREQ(BasicTypeToString(typeid(int16_t)).data(),"int16_t");
	EXPECT_STREQ(BasicTypeToString(typeid(int32_t)).data(), "int32_t");
	EXPECT_STREQ(BasicTypeToString(typeid(int64_t)).data(), "int64_t");
	EXPECT_STREQ(BasicTypeToString(typeid(float)).data(), "float");
	EXPECT_STREQ(BasicTypeToString(typeid(double)).data(), "double");
	EXPECT_STREQ(BasicTypeToString(typeid(uint8_t*)).data(), "uint8_t*");
	EXPECT_STREQ(BasicTypeToString(typeid(uint16_t*)).data(), "uint16_t*");
	EXPECT_STREQ(BasicTypeToString(typeid(uint32_t*)).data(), "uint32_t*");
	EXPECT_STREQ(BasicTypeToString(typeid(uint64_t*)).data(), "uint64_t*");
	EXPECT_STREQ(BasicTypeToString(typeid(int8_t*)).data(), "int8_t*");
	EXPECT_STREQ(BasicTypeToString(typeid(int16_t*)).data(), "int16_t*");
	EXPECT_STREQ(BasicTypeToString(typeid(int32_t*)).data(), "int32_t*");
	EXPECT_STREQ(BasicTypeToString(typeid(int64_t*)).data(), "int64_t*");
	EXPECT_STREQ(BasicTypeToString(typeid(float*)).data(), "float*");
	EXPECT_STREQ(BasicTypeToString(typeid(double*)).data(), "double*");
	EXPECT_THROW(BasicTypeToString(typeid(HANDLE)), std::invalid_argument);
}

TEST(TypeHelpTests, IsBasicType)
{
	EXPECT_FALSE(IsBasicType(typeid(uint8_t*)));
	EXPECT_FALSE(IsBasicType(typeid(uint16_t*)));
	EXPECT_FALSE(IsBasicType(typeid(uint32_t*)));
	EXPECT_FALSE(IsBasicType(typeid(uint64_t*)));

	EXPECT_FALSE(IsBasicType(typeid(int8_t*)));
	EXPECT_FALSE(IsBasicType(typeid(int16_t*)));
	EXPECT_FALSE(IsBasicType(typeid(int32_t*)));
	EXPECT_FALSE(IsBasicType(typeid(int64_t*)));

	EXPECT_FALSE(IsBasicType(typeid(float*)));
	EXPECT_FALSE(IsBasicType(typeid(float*)));
	EXPECT_FALSE(IsBasicType(typeid(double*)));

	EXPECT_TRUE(IsBasicType(typeid(uint8_t)));
	EXPECT_TRUE(IsBasicType(typeid(uint16_t)));
	EXPECT_TRUE(IsBasicType(typeid(uint32_t)));
	EXPECT_TRUE(IsBasicType(typeid(uint64_t)));

	EXPECT_TRUE(IsBasicType(typeid(int8_t)));
	EXPECT_TRUE(IsBasicType(typeid(int16_t)));
	EXPECT_TRUE(IsBasicType(typeid(int32_t)));
	EXPECT_TRUE(IsBasicType(typeid(int64_t)));

	EXPECT_TRUE(IsBasicType(typeid(float)));
	EXPECT_TRUE(IsBasicType(typeid(float)));
	EXPECT_TRUE(IsBasicType(typeid(double)));

	EXPECT_FALSE(IsBasicType(typeid(HANDLE)));
}

TEST(TypeHelpTests, IsPointer)
{
	EXPECT_TRUE(IsBasicPointer(typeid(uint8_t*)));
	EXPECT_TRUE(IsBasicPointer(typeid(uint16_t*)));
	EXPECT_TRUE(IsBasicPointer(typeid(uint32_t*)));
	EXPECT_TRUE(IsBasicPointer(typeid(uint64_t*)));

	EXPECT_TRUE(IsBasicPointer(typeid(int8_t*)));
	EXPECT_TRUE(IsBasicPointer(typeid(int16_t*)));
	EXPECT_TRUE(IsBasicPointer(typeid(int32_t*)));
	EXPECT_TRUE(IsBasicPointer(typeid(int64_t*)));

	EXPECT_TRUE(IsBasicPointer(typeid(float*)));
	EXPECT_TRUE(IsBasicPointer(typeid(float*)));
	EXPECT_TRUE(IsBasicPointer(typeid(double*)));

	EXPECT_FALSE(IsBasicPointer(typeid(uint8_t)));
	EXPECT_FALSE(IsBasicPointer(typeid(uint16_t)));
	EXPECT_FALSE(IsBasicPointer(typeid(uint32_t)));
	EXPECT_FALSE(IsBasicPointer(typeid(uint64_t)));

	EXPECT_FALSE(IsBasicPointer(typeid(int8_t)));
	EXPECT_FALSE(IsBasicPointer(typeid(int16_t)));
	EXPECT_FALSE(IsBasicPointer(typeid(int32_t)));
	EXPECT_FALSE(IsBasicPointer(typeid(int64_t)));

	EXPECT_FALSE(IsBasicPointer(typeid(float)));
	EXPECT_FALSE(IsBasicPointer(typeid(float)));
	EXPECT_FALSE(IsBasicPointer(typeid(double)));

	EXPECT_FALSE(IsBasicPointer(typeid(HANDLE)));
}

TEST(TypeHelpTests, Ordering)
{
	EXPECT_LT(HacklibOrder(typeid(uint8_t)), HacklibOrder(typeid(uint64_t)));
	EXPECT_LT(HacklibOrder(typeid(int8_t)), HacklibOrder(typeid(int64_t)));
	EXPECT_LT(HacklibOrder(typeid(float)), HacklibOrder(typeid(double)));
	EXPECT_LT(HacklibOrder(typeid(uint8_t*)), HacklibOrder(typeid(uint64_t*)));
	EXPECT_LT(HacklibOrder(typeid(int8_t*)), HacklibOrder(typeid(int64_t*)));
	EXPECT_LT(HacklibOrder(typeid(float*)), HacklibOrder(typeid(double*)));
}