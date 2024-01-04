#include "MemoryLeakListener.hpp"

int wmain(int argc, wchar_t** argv)
{
	int verbosity = 0;

	for (int i = 1; i < argc; ++i)
	{
		const wchar_t* argument = argv[i];

		if (_wcsicmp(L"--verbosity=error", argument) == 0)
		{
			verbosity = 1;
			break;
		}

		if (_wcsicmp(L"--verbosity=info", argument) == 0)
		{
			verbosity = 2;
			break;
		}

		if (_wcsicmp(L"--verbosity=debug", argument) == 0)
		{
			verbosity = 3;
			break;
		}
	}

	switch (verbosity)
	{
		case 0:
			std::clog.setstate(std::ios::failbit);
			std::cout.setstate(std::ios::failbit);
			std::cerr.setstate(std::ios::failbit);
			break;
		case 1:
			std::clog.setstate(std::ios::failbit);
			std::cout.setstate(std::ios::failbit);
			break;
		case 2:
			std::clog.setstate(std::ios::failbit);
			break;
		case 3:
			break;
		default:
			return ERROR_BAD_ARGUMENTS;
	}

	testing::InitGoogleTest(&argc, argv);

#if defined(_DEBUG)
	testing::UnitTest::GetInstance()->listeners().Append(new MemoryLeakListener());
#endif

	int result = testing::UnitTest::GetInstance()->Run();

	return result;
}