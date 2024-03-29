#include "Win32Thread.hpp"

TEST(ThreadTests, Creation)
{
	uint32_t i = 0;

	auto function = [&](const Win32Event&)->uint32_t
	{ 
		return ++i;
	};

	for (size_t x = 0; x < 100; ++x)
	{
		Win32Thread thread(function);
	}

	EXPECT_EQ(i, 100u);
}

TEST(ThreadTests, ExitCode)
{
	size_t i = 0;

	auto function = [&](const Win32Event&)->uint32_t
	{
		while (i < 1000)
		{
			++i;
		}

		return 0x1337C0D3;
	};

	Win32Thread thread(function);
	EXPECT_EQ(thread.ExitCode(), STILL_ACTIVE);

	thread.Wait(5ms);

	EXPECT_EQ(thread.ExitCode(), 0x1337C0D3);
	EXPECT_EQ(i, 1000u);
}

TEST(ThreadTests, Abandon)
{
	size_t i = 0;

	auto function = [&](const Win32Event& event)->uint32_t
	{
		while (!event.IsSet())
		{
			++i;
		}

		return 0;
	};

	{
		Win32Thread thread(function);
		EXPECT_EQ(thread.Wait(2ms), WAIT_TIMEOUT);
	}

	EXPECT_GT(i, 0u);
}

TEST(ThreadTests, Identifier)
{
	size_t i = 0;

	auto function = [&](const Win32Event& event)->uint32_t
	{
		while (!event.IsSet())
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

		Sleep(2);
	}

	EXPECT_GT(i, 0u);
}