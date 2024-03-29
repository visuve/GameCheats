#include "Win32Thread.hpp"

TEST(ThreadTests, Basic)
{
	size_t i = 0;

	auto function = [&](std::stop_token)->uint32_t
	{
		while (i < 10)
		{
			++i;
		}

		return 0xABCDEFu;
	};

	Win32Thread thread(function);

	thread.Wait(100ms);

	EXPECT_EQ(thread.ExitCode(), 0xABCDEFu);
	EXPECT_EQ(i, 10u);
}

TEST(ThreadTests, Abandon)
{
	size_t i = 0;

	auto function = [&](std::stop_token token)->uint32_t
	{
		while (!token.stop_requested())
		{
			++i;
		}

		return 0;
	};

	{
		Win32Thread thread(function);
		EXPECT_THROW(thread.Wait(1ms), std::system_error);
	}

	EXPECT_GT(i, 0u);
}

TEST(ThreadTests, Identifier)
{
	size_t i = 0;

	auto function = [&](std::stop_token token)->uint32_t
	{
		while (!token.stop_requested())
		{
			++i;
		}

		return 0;
	};

	{
		Win32Thread thread(function);
		DWORD threadID = thread.Identifier();

		Win32Thread clone(threadID);
		DWORD cloneID = clone.Identifier();

		EXPECT_EQ(threadID, cloneID);

		Sleep(1);
	}

	EXPECT_GT(i, 0u);
}