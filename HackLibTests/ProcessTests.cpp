#include "Process.hpp"

TEST(ProcessTests, Header)
{
	DWORD pid = GetCurrentProcessId();
	Process current(pid);

	EXPECT_EQ(current.NtHeader().OptionalHeader.Subsystem, WORD(IMAGE_SUBSYSTEM_WINDOWS_CUI));
}

TEST(ProcessTests, Name)
{
	DWORD pid = GetCurrentProcessId();
	Process current(pid);

	EXPECT_STREQ(current.Path().filename().c_str(), L"HackLibTests.exe");
}

TEST(ProcessTests, ModuleNotFound)
{
	DWORD pid = GetCurrentProcessId();
	Process currentProcess(pid);

	EXPECT_THROW(currentProcess.FindFunction(
		"This module does not exists",
		"Neither does this function"), std::range_error);
}

TEST(ProcessTests, FunctionNotFound)
{
	DWORD pid = GetCurrentProcessId();
	Process currentProcess(pid);

	EXPECT_THROW(currentProcess.FindFunction(
		"KERNEL32.dll",
		"Neither does this function"), std::range_error);
}

TEST(ProcessTests, FunctionFound)
{
	DWORD pid = GetCurrentProcessId();
	Process currentProcess(pid);

	// It's just called above. It has to be found
	Pointer fnptr = 
		currentProcess.FindFunction("KERNEL32.dll", "GetCurrentProcessId");

	EXPECT_NE(fnptr, Pointer());
}

TEST(ProcessTests, Timeout)
{
	using namespace std::chrono_literals;

	DWORD pid = GetCurrentProcessId();
	Process current(pid);

	EXPECT_EQ(current.WairForExit(0ms), DWORD(WAIT_TIMEOUT));
	EXPECT_EQ(current.WairForExit(1ms), DWORD(WAIT_TIMEOUT));
	EXPECT_EQ(current.WairForExit(2ms), DWORD(WAIT_TIMEOUT));
}

#ifndef _WIN64
TEST(ProcessTests, Jump)
{
	{
		auto jump = Process::JumpOp(
			reinterpret_cast<uint8_t*>(0x009A0000),
			reinterpret_cast<uint8_t*>(0x009A0201));

		EXPECT_EQ(jump[0], 0xE9u);
		EXPECT_EQ(jump[1], 0xFCu);
		EXPECT_EQ(jump[2], 0x01u);
		EXPECT_EQ(jump[3], 0x00u);
		EXPECT_EQ(jump[4], 0x00u);
	}
	{
		auto jump = Process::JumpOp(
			reinterpret_cast<uint8_t*>(0x009A0201),
			reinterpret_cast<uint8_t*>(0x009A0000));

		EXPECT_EQ(jump[0], 0xE9u);
		EXPECT_EQ(jump[1], 0xFAu);
		EXPECT_EQ(jump[2], 0xFDu);
		EXPECT_EQ(jump[3], 0xFFu);
		EXPECT_EQ(jump[4], 0xFFu);
	}
}
#endif