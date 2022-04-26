#include "SHA256.hpp"

TEST(ChecksumTests, Self)
{
	std::wstring path = GetCommandLineW() + 1;

	// The command line seems to have some goo by default
	path.pop_back();
	path.pop_back();

	EXPECT_TRUE(std::filesystem::exists(path));

	EXPECT_EQ(SHA256(path).Value().size(), size_t(64));
}

TEST(ChecksumTests, PathNotExists)
{
	EXPECT_THROW(SHA256(L"This path does not exist"), std::system_error);
}

TEST(ChecksumTests, InvalidCompare)
{
	std::wstring path = GetCommandLineW() + 1;

	path.pop_back();
	path.pop_back();

	EXPECT_THROW(SHA256(path) == "Not a valid SHA-256", std::logic_error);
}