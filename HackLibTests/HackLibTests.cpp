int wmain()
{
	std::cout.setstate(std::ios::failbit);
	testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}