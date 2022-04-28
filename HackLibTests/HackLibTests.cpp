#include "MemoryLeakListener.hpp"

int wmain(int argc, wchar_t** argv)
{
	std::cout.setstate(std::ios::failbit);

#if defined(_DEBUG)
	testing::UnitTest::GetInstance()->listeners().Append(new MemoryLeakListener());
#endif

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}