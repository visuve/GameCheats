#include "MemoryLeakListener.hpp"

int wmain()
{
	std::cout.setstate(std::ios::failbit);

#if defined(_DEBUG)
	testing::UnitTest::GetInstance()->listeners().Append(new MemoryLeakListener());
#endif

	testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}