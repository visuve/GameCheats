#include "SHA256.hpp"
#include "System.hpp"

TEST(ChecksumTests, Self)
{
	auto exe = System::CurrentExecutablePath();
	EXPECT_EQ(SHA256(exe).Value().size(), SHA256::HashCharacters);
}

TEST(ChecksumTests, PathNotExists)
{
	EXPECT_THROW(SHA256(L"This path does not exist"), std::system_error);
}

TEST(ChecksumTests, InvalidCompare)
{
	auto exe = System::CurrentExecutablePath();
	EXPECT_THROW(SHA256(exe) == "Not a valid SHA-256", std::logic_error);
}