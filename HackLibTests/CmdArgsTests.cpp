#include "CmdArgs.hpp"

TEST(CmdArgsTests, ContainsSimple)
{
	constexpr int argc = 2;

	const wchar_t* argv[argc] =
	{
		L"foo",
		L"bar"
	};

	const CmdArgs args(argc, const_cast<wchar_t**>(argv),
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
	constexpr int argc = 2;

	const wchar_t* argv[argc] =
	{
		L"foobar",
		L"barfoo"
	};

	EXPECT_THROW(CmdArgs(argc, const_cast<wchar_t**>(argv),
	{
		{ L"foo", typeid(std::nullopt), L"Foos, not bars. Definetely not foobars" },
		{ L"bar", typeid(std::nullopt), L"Bars, not foos. Definetely not barfoos" }
	}), CmdArgs::Exception);
}

TEST(CmdArgsTests, ContainsValuedArgument)
{
	constexpr int argc = 2;

	const wchar_t* argv[argc] =
	{
		L"foo=123",
		L"bar=456"
	};

	const CmdArgs args(argc, const_cast<wchar_t**>(argv),
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
	constexpr int argc = 5;

	wchar_t* argv[argc] =
	{
		_wcsdup(L"alpha"),
		_wcsdup(L"bravo=nonexistent/path"),
		_wcsdup(L"charlie=3.14159265359"),
		_wcsdup(L"delta=3.14159265359"),
		_wcsdup(L"echo=3.14159265359"),
	};

	const CmdArgs args(argc, argv,
	{
		{ L"alpha", typeid(std::nullopt), L"Null option" },
		{ L"bravo", typeid(std::filesystem::path), L"A path" },
		{ L"charlie", typeid(double), L"A double value" },
		{ L"delta", typeid(float), L"A float value" },
		{ L"echo", typeid(int), L"An integer value" }
	});

	EXPECT_TRUE(args.Contains(L"alpha"));
	EXPECT_TRUE(args.Contains(L"bravo"));
	EXPECT_TRUE(args.Contains(L"charlie"));
	EXPECT_TRUE(args.Contains(L"delta"));
	EXPECT_TRUE(args.Contains(L"echo"));

	EXPECT_TRUE(args.Get<bool>(L"alpha"));

	auto path = args.Get<std::filesystem::path>(L"bravo");
	EXPECT_STREQ(path.c_str(), L"nonexistent/path");
	EXPECT_EQ(args.Get<double>(L"charlie"), double(3.14159265359));
	EXPECT_EQ(args.Get<float>(L"delta"), float(3.14159265359));
	EXPECT_EQ(args.Get<int>(L"echo"), int(3));

	for (int i = 0; i < argc; ++i)
	{
		delete argv[i];
	}
}

TEST(CmdArgsTests, InvalidType)
{
	constexpr int argc = 5;

	wchar_t* argv[argc] =
	{
		_wcsdup(L"alpha"),
		_wcsdup(L"bravo=nonexistent/path"),
		_wcsdup(L"charlie=3.14159265359"),
		_wcsdup(L"delta=3.14159265359"),
		_wcsdup(L"echo=3.14159265359"),
	};

	const CmdArgs args(argc, argv,
	{
		{ L"alpha", typeid(std::nullopt), L"Null option" },
		{ L"bravo", typeid(std::filesystem::path), L"A path" },
		{ L"charlie", typeid(double), L"A double value" },
		{ L"delta", typeid(float), L"A float value" },
		{ L"echo", typeid(int), L"An integer value" }
	});

	EXPECT_THROW(args.Get<std::filesystem::path>(L"alpha"), std::bad_any_cast);
	EXPECT_THROW(args.Get<double>(L"bravo"), std::bad_any_cast);
	EXPECT_THROW(args.Get<float>(L"charlie"), std::bad_any_cast);
	EXPECT_THROW(args.Get<int>(L"delta"), std::bad_any_cast);
	EXPECT_THROW(args.Get<bool>(L"echo"), std::bad_any_cast);

	for (int i = 0; i < argc; ++i)
	{
		delete argv[i];
	}
}

TEST(CmdArgsTests, InvalidFormat)
{
	constexpr int argc = 5;

	wchar_t* argv[argc] =
	{
		_wcsdup(L"alpha-"),
		_wcsdup(L"bravo==nonexistent/path"),
		_wcsdup(L"charlie:3.14159265359"),
		_wcsdup(L"delta#3.14159265359"),
		_wcsdup(L"echo@3.14159265359"),
	};

	const CmdArgs args(argc, argv,
	{
		{ L"alpha", typeid(std::nullopt), L"Null option" },
		{ L"bravo", typeid(std::filesystem::path), L"A path" },
		{ L"charlie", typeid(double), L"A double value" },
		{ L"delta", typeid(float), L"A float value" },
		{ L"echo", typeid(int), L"An integer value" }
	});

	EXPECT_THROW(args.Get<bool>(L"alpha"), CmdArgs::Exception);

	// I need one "valid" argument for the CmdArgs ctor not to throw
	auto path = args.Get<std::filesystem::path>(L"bravo");
	EXPECT_STREQ(path.c_str(), L"=nonexistent/path");

	EXPECT_THROW(args.Get<double>(L"charlie"), CmdArgs::Exception);
	EXPECT_THROW(args.Get<float>(L"delta"), CmdArgs::Exception);
	EXPECT_THROW(args.Get<int>(L"echo"), CmdArgs::Exception);

	for (int i = 0; i < argc; ++i)
	{
		delete argv[i];
	}
}