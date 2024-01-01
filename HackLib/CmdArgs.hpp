#pragma once

#include "NonCopyable.hpp"
#include "Exceptions.hpp"

class CmdArgs
{
public:
	class Exception : public std::exception
	{
	public:
		Exception(const std::string& what, const std::string& usage);
		std::string_view Usage() const;
		const char* what() const throw ();

	private:
		const std::string _usage;
		const std::string _what;
	};

	struct Argument
	{
		std::string Key;
		std::type_index Type;
		std::string Description;
	};

	CmdArgs(const std::vector<std::string>& given, std::initializer_list<Argument> expected);
	CmdArgs(int argc, char** argv, std::initializer_list<Argument> expected);
	NonCopyable(CmdArgs);

	bool Contains(std::string_view key) const;

	template<typename T>
	T Value(std::string_view key) const
	{
		return std::any_cast<T>(ValueByKey(key));
	}

	template<typename T>
	T Value(std::string_view key, const T& defaultValue) const
	{
		return Contains(key) ? Value<T>(key) : defaultValue;
	}

	std::string Usage() const;

private:
	bool AppearsValid(std::string_view raw) const;
	std::type_index TypeByKey(std::string_view key) const;
	std::any ValueByKey(std::string_view key) const;

	std::string _usage;
	const std::vector<Argument> _expected;
	const std::vector<std::string> _givenArguments;
};