#include "System.hpp"

TEST(SystemTests, Guid)
{
	auto a = System::GenerateGuid();
	auto b = System::GenerateGuid();

	EXPECT_NE(a, b);
	EXPECT_EQ(a.size(), 38u);
	EXPECT_EQ(b.size(), 38u);
}

TEST(SystemTests, PageSize)
{
	// https://devblogs.microsoft.com/oldnewthing/20210510-00/?p=105200
	EXPECT_GE(System::PageSize(), 0x1000u);
}

TEST(SystemTests, Sanity)
{
	DWORD pid = System::WaitForExe(L"HackLibTests.exe");
	EXPECT_NE(pid, DWORD(0));

	{
		MODULEENTRY32W module = System::ModuleEntryByName(pid, L"KERNEL32.dll");
		EXPECT_EQ(pid, module.th32ProcessID);
	}
	{
		MODULEENTRY32W module = System::ModuleEntryByPid(pid);
		EXPECT_EQ(pid, module.th32ProcessID);
	}
}

TEST(SystemTests, ModuleEntryNotFound)
{
	{
		DWORD pid = System::WaitForExe(L"HackLibTests.exe");

		EXPECT_NE(pid, DWORD(0));

		EXPECT_THROW(
			System::ModuleEntryByName(pid, L"This does not exist.dll"), std::range_error);
	}
	{
		EXPECT_THROW(System::ModuleEntryByPid(0), std::system_error);
		EXPECT_THROW(System::ModuleEntryByPid(0xFFFFFFFF), std::system_error);
	}
	{
		EXPECT_THROW(System::PidByName(
			L"This certainly does not exist 123456789.exe"), std::range_error);
	}
}