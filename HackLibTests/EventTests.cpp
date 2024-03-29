#include "Win32Event.hpp"

TEST(EventTests, Basic)
{
	Win32Event event;
	EXPECT_FALSE(event.IsSet());
	event.Set();
	EXPECT_TRUE(event.IsSet());
}

TEST(EventTests, Wait)
{
	Win32Event event;
	EXPECT_EQ(event.Wait(1ms), WAIT_TIMEOUT);
}