#include "MemoryLeakListener.hpp"

MemoryLeakListener::MemoryLeakListener() :
	testing::EmptyTestEventListener()
{
	for (int mode : { _CRT_WARN, _CRT_ERROR, _CRT_ASSERT })
	{
		_CrtSetReportMode(mode, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(mode, _CRTDBG_FILE_STDERR);
	}
}

MemoryLeakListener::~MemoryLeakListener()
{
}

void MemoryLeakListener::OnTestStart(const testing::TestInfo&)
{
	_testBegin = {};
	_CrtMemCheckpoint(&_testBegin);
}

void MemoryLeakListener::OnTestEnd(const testing::TestInfo& testInfo)
{
	if (!testInfo.result()->Passed())
	{
		return;
	}

	_CrtMemState testEnd = {};
	_CrtMemCheckpoint(&testEnd);

	_CrtMemState diff = {};
	int significantlyDifferent = _CrtMemDifference(&diff, &_testBegin, &testEnd);

	if (significantlyDifferent != 0)
	{
		_CrtMemDumpStatistics(&diff);
	}

	EXPECT_LE(significantlyDifferent, 0) << "Probable memory leak detected";
}