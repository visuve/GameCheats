#include "SHA256.hpp"

struct ChecksumTests : ::testing::Test
{
	ChecksumTests() :
		Test()
	{
		// https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
		_path.resize(0x7FFFu);
		size_t required = GetModuleFileNameW(nullptr, _path.data(), 0x7FFFu);
		_path.resize(required);
	}

	std::wstring _path;
};

TEST_F(ChecksumTests, Self)
{
	EXPECT_EQ(SHA256(_path).Value().size(), SHA256::HashCharacters);
}

TEST_F(ChecksumTests, PathNotExists)
{
	EXPECT_THROW(SHA256(L"This path does not exist"), std::system_error);
}

TEST_F(ChecksumTests, InvalidCompare)
{
	EXPECT_THROW(SHA256(_path) == "Not a valid SHA-256", std::logic_error);
}