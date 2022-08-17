#include "CmdArgs.hpp"

TEST(CmdArgsTests, ContainsSimple)
{
	const CmdArgs args({ "foo", "bar" },
	{
		{ "foo", typeid(std::nullopt), "Foos, not bars" },
		{ "bar", typeid(std::nullopt), "Bars, not foos" }
	});

	EXPECT_TRUE(args.Contains("foo"));
	EXPECT_TRUE(args.Contains("bar"));
	EXPECT_FALSE(args.Contains("foobar"));
}

TEST(CmdArgsTests, Missing)
{
	EXPECT_THROW(CmdArgs({ "foobar", "barfoo" },
	{
		{ "foo", typeid(std::nullopt), "Foos, not bars. Definetely not foobars" },
		{ "bar", typeid(std::nullopt), "Bars, not foos. Definetely not barfoos" }
	}), CmdArgs::Exception);
}

TEST(CmdArgsTests, ContainsValuedArgument)
{
	const CmdArgs args({ "foo=123", "bar=456" },
	{
		{ "foo", typeid(int), "Foos, not bars" },
		{ "bar", typeid(int), "Bars, not foos" }
	});

	EXPECT_TRUE(args.Contains("foo"));
	EXPECT_TRUE(args.Contains("bar"));
	EXPECT_FALSE(args.Contains("foobar"));
}

TEST(CmdArgsTests, Default)
{
	const CmdArgs args({ "foo=123", "bar=456" },
	{
		{ "foo", typeid(int), "Foos, not bars" },
		{ "bar", typeid(int), "Bars, not foos" }
	});

	EXPECT_EQ(args.Value<int>("foo"), 123);
	EXPECT_EQ(args.Value<int>("foo", 789), 123);
	EXPECT_EQ(args.Value<int>("bar"), 456);
	EXPECT_EQ(args.Value<int>("bar", 789), 456);
	EXPECT_EQ(args.Value<int>("foobar", 789), 789);
}

TEST(CmdArgsTests, ParseArgument)
{
	const std::vector<std::string> given =
	{
		"alpha",
		"bravo=nonexistent/path",
		"charlie=3.14159265359",
		"delta=3.14159265359",
		"echo=3.14159265359",
		"foxtrot=foobar"
	};

	const CmdArgs args(given,
	{
		{ "alpha", typeid(std::nullopt), "Null option" },
		{ "bravo", typeid(std::filesystem::path), "A path" },
		{ "charlie", typeid(double), "A double value" },
		{ "delta", typeid(float), "A float value" },
		{ "echo", typeid(int), "An integer value" },
		{ "foxtrot", typeid(std::string), "A string value" },
	});

	EXPECT_TRUE(args.Contains("alpha"));
	EXPECT_TRUE(args.Contains("bravo"));
	EXPECT_TRUE(args.Contains("charlie"));
	EXPECT_TRUE(args.Contains("delta"));
	EXPECT_TRUE(args.Contains("echo"));
	EXPECT_TRUE(args.Contains("foxtrot"));

	EXPECT_TRUE(args.Value<bool>("alpha"));

	auto path = args.Value<std::filesystem::path>("bravo");
	EXPECT_STREQ(path.c_str(), L"nonexistent/path");
	EXPECT_EQ(args.Value<double>("charlie"), double(3.14159265359));
	EXPECT_EQ(args.Value<float>("delta"), float(3.14159265359));
	EXPECT_EQ(args.Value<int>("echo"), int(3));

	auto string = args.Value<std::string>("foxtrot");
	EXPECT_STREQ(string.c_str(), "foobar");
}

TEST(CmdArgsTests, InvalidType)
{
	const std::vector<std::string> given =
	{
		"alpha",
		"bravo=nonexistent/path",
		"charlie=3.14159265359",
		"delta=3.14159265359",
		"echo=3.14159265359",
		"foxtrot=foobar"
	};

	const CmdArgs args(given,
	{
		{ "alpha", typeid(std::nullopt), "Null option" },
		{ "bravo", typeid(std::filesystem::path), "A path" },
		{ "charlie", typeid(double), "A double value" },
		{ "delta", typeid(float), "A float value" },
		{ "echo", typeid(int), "An integer value" },
		{ "foxtrot", typeid(std::string), "A string value" }
	});

	EXPECT_THROW(args.Value<std::filesystem::path>("alpha"), std::bad_any_cast);
	EXPECT_THROW(args.Value<std::string>("bravo"), std::bad_any_cast);
	EXPECT_THROW(args.Value<float>("charlie"), std::bad_any_cast);
	EXPECT_THROW(args.Value<int>("delta"), std::bad_any_cast);
	EXPECT_THROW(args.Value<bool>("echo"), std::bad_any_cast);
	EXPECT_THROW(args.Value<std::filesystem::path>("foxtrot"), std::bad_any_cast);
	EXPECT_THROW(args.Value<std::wstring>("foxtrot"), std::bad_any_cast);
}

TEST(CmdArgsTests, InvalidFormat)
{
	const std::vector<std::string> given =
	{
		"alpha-",
		"bravo==nonexistent/path",
		"charlie:3.14159265359",
		"delta#3.14159265359",
		"echo@3.14159265359",
		"foxtrot foobar"
	};

	const CmdArgs args(given,
	{
		{ "alpha", typeid(std::nullopt), "Null option" },
		{ "bravo", typeid(std::filesystem::path), "A path" },
		{ "charlie", typeid(double), "A double value" },
		{ "delta", typeid(float), "A float value" },
		{ "echo", typeid(int), "An integer value" },
		{ "foxtrot", typeid(std::string), "A string value" }
	});

	EXPECT_THROW(args.Value<bool>("alpha"), CmdArgs::Exception);

	// I need one "valid" argument for the CmdArgs ctor not to throw
	auto path = args.Value<std::filesystem::path>("bravo");
	EXPECT_STREQ(path.c_str(), L"=nonexistent/path");

	EXPECT_THROW(args.Value<double>("charlie"), CmdArgs::Exception);
	EXPECT_THROW(args.Value<float>("delta"), CmdArgs::Exception);
	EXPECT_THROW(args.Value<int>("echo"), CmdArgs::Exception);
	EXPECT_THROW(args.Value<std::string>("foxtrot"), CmdArgs::Exception);
}