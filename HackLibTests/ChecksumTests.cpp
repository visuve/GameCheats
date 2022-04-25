#include "SHA256.hpp"

TEST(ChecksumTests, GTestZip)
{
	// NOTE: this test will fail if run outside the default build directory

	std::filesystem::path path(GetCommandLineW() + 1);
	path.remove_filename();
	path /= "_deps\\googletest-subbuild\\googletest-populate-prefix\\src\\release-1.11.0.zip";

	EXPECT_TRUE(std::filesystem::exists(path));

	EXPECT_TRUE(SHA256(path, nullptr) == "353571c2440176ded91c2de6d6cd88ddd41401d14692ec1f99e35d013feda55a");
}