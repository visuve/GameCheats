#include "CmdArgs.hpp"

TEST(CmdArgsTests, ContainsSimple)
{
	const CmdArgs args({ L"foo", L"bar" },
	{
		{ L"foo", typeid(std::nullopt), L"Foos, not bars" },
		{ L"bar", typeid(std::nullopt), L"Bars, not foos" }
	});

	EXPECT_TRUE(args.Contains(L"foo"));
	EXPECT_TRUE(args.Contains(L"bar"));
	EXPECT_FALSE(args.Contains(L"foobar"));
}

TEST(CmdArgsTests, Missing)
{
	EXPECT_THROW(CmdArgs({ L"foobar", L"barfoo" },
	{
		{ L"foo", typeid(std::nullopt), L"Foos, not bars. Definetely not foobars" },
		{ L"bar", typeid(std::nullopt), L"Bars, not foos. Definetely not barfoos" }
	}), CmdArgs::Exception);
}

TEST(CmdArgsTests, ContainsValuedArgument)
{
	const CmdArgs args({ L"foo=123", L"bar=456" },
	{
		{ L"foo", typeid(int), L"Foos, not bars" },
		{ L"bar", typeid(int), L"Bars, not foos" }
	});

	EXPECT_TRUE(args.Contains(L"foo"));
	EXPECT_TRUE(args.Contains(L"bar"));
	EXPECT_FALSE(args.Contains(L"foobar"));
}

TEST(CmdArgsTests, ParseArgument)
{
	const std::vector<std::wstring> given =
	{
		L"alpha",
		L"bravo=nonexistent/path",
		L"charlie=3.14159265359",
		L"delta=3.14159265359",
		L"echo=3.14159265359",
		L"foxtrot=foobar"
	};

	const CmdArgs args(given,
	{
		{ L"alpha", typeid(std::nullopt), L"Null option" },
		{ L"bravo", typeid(std::filesystem::path), L"A path" },
		{ L"charlie", typeid(double), L"A double value" },
		{ L"delta", typeid(float), L"A float value" },
		{ L"echo", typeid(int), L"An integer value" },
		{ L"foxtrot", typeid(std::wstring), L"A string value" },
	});

	EXPECT_TRUE(args.Contains(L"alpha"));
	EXPECT_TRUE(args.Contains(L"bravo"));
	EXPECT_TRUE(args.Contains(L"charlie"));
	EXPECT_TRUE(args.Contains(L"delta"));
	EXPECT_TRUE(args.Contains(L"echo"));
	EXPECT_TRUE(args.Contains(L"foxtrot"));

	EXPECT_TRUE(args.Value<bool>(L"alpha"));

	auto path = args.Value<std::filesystem::path>(L"bravo");
	EXPECT_STREQ(path.c_str(), L"nonexistent/path");
	EXPECT_EQ(args.Value<double>(L"charlie"), double(3.14159265359));
	EXPECT_EQ(args.Value<float>(L"delta"), float(3.14159265359));
	EXPECT_EQ(args.Value<int>(L"echo"), int(3));

	auto string = args.Value<std::wstring>(L"foxtrot");
	EXPECT_STREQ(string.c_str(), L"foobar");
}

TEST(CmdArgsTests, InvalidType)
{
	const std::vector<std::wstring> given =
	{
		L"alpha",
		L"bravo=nonexistent/path",
		L"charlie=3.14159265359",
		L"delta=3.14159265359",
		L"echo=3.14159265359",
		L"foxtrot=foobar"
	};

	const CmdArgs args(given,
	{
		{ L"alpha", typeid(std::nullopt), L"Null option" },
		{ L"bravo", typeid(std::filesystem::path), L"A path" },
		{ L"charlie", typeid(double), L"A double value" },
		{ L"delta", typeid(float), L"A float value" },
		{ L"echo", typeid(int), L"An integer value" },
		{ L"foxtrot", typeid(std::wstring), L"A string value" }
	});

	EXPECT_THROW(args.Value<std::filesystem::path>(L"alpha"), std::bad_any_cast);
	EXPECT_THROW(args.Value<std::wstring>(L"bravo"), std::bad_any_cast);
	EXPECT_THROW(args.Value<float>(L"charlie"), std::bad_any_cast);
	EXPECT_THROW(args.Value<int>(L"delta"), std::bad_any_cast);
	EXPECT_THROW(args.Value<bool>(L"echo"), std::bad_any_cast);
	EXPECT_THROW(args.Value<std::filesystem::path>(L"foxtrot"), std::bad_any_cast);
}

TEST(CmdArgsTests, InvalidFormat)
{
	const std::vector<std::wstring> given =
	{
		L"alpha-",
		L"bravo==nonexistent/path",
		L"charlie:3.14159265359",
		L"delta#3.14159265359",
		L"echo@3.14159265359",
		L"foxtrot foobar"
	};

	const CmdArgs args(given,
	{
		{ L"alpha", typeid(std::nullopt), L"Null option" },
		{ L"bravo", typeid(std::filesystem::path), L"A path" },
		{ L"charlie", typeid(double), L"A double value" },
		{ L"delta", typeid(float), L"A float value" },
		{ L"echo", typeid(int), L"An integer value" },
		{ L"foxtrot", typeid(std::wstring), L"A string value" }
	});

	EXPECT_THROW(args.Value<bool>(L"alpha"), CmdArgs::Exception);

	// I need one "valid" argument for the CmdArgs ctor not to throw
	auto path = args.Value<std::filesystem::path>(L"bravo");
	EXPECT_STREQ(path.c_str(), L"=nonexistent/path");

	EXPECT_THROW(args.Value<double>(L"charlie"), CmdArgs::Exception);
	EXPECT_THROW(args.Value<float>(L"delta"), CmdArgs::Exception);
	EXPECT_THROW(args.Value<int>(L"echo"), CmdArgs::Exception);
	EXPECT_THROW(args.Value<std::wstring>(L"foxtrot"), CmdArgs::Exception);
}