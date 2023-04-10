#include "PE.hpp"
#include "System.hpp"

TEST(PETests, ImportFound)
{
	Sleep(1);

	PE::Executable current(L"HackLibTests.exe");

	auto imported = current.ImportedFunctions("KERNEL32.dll");

	// I am calling Sleep just above, so there has to be the Sleep function import
	EXPECT_NE(std::find(imported.begin(), imported.end(), "Sleep"), imported.end());
}

TEST(PETests, ImportedLibraryNotFound)
{
	PE::Executable current(L"HackLibTests.exe");

	auto imported = current.ImportedFunctions("I surely do not exist.dll");

	EXPECT_TRUE(imported.empty());
}

TEST(PETests, ExportFound)
{
	auto path = System::SystemDirectory() / "icmp.dll";
	PE::Library library(path);

	auto exported = library.ExportedFunctions();

	EXPECT_FALSE(exported.empty());

	EXPECT_NE(std::find(exported.begin(), exported.end(), "IcmpSendEcho"), exported.end());
}