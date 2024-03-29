#pragma once

#include "NonCopyable.hpp"
#include "Exceptions.hpp"

class CommandLine
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

	class Argument
	{
	public:
		Argument(const std::string& key, const std::type_index type, const std::string& description);

		bool Parse(std::string_view value);

		const std::string Key;
		const std::type_index Type;
		const std::string Description;
		std::any Value;
	};

	CommandLine(const std::vector<std::string>& given, std::initializer_list<Argument> expected);
	CommandLine(int argc, char** argv, std::initializer_list<Argument> expected);
	NonCopyable(CommandLine);

	bool Contains(std::string_view key) const;

	template<typename T>
	T Value(std::string_view key) const
	{
		const auto it = Get(key);

		if (it == _arguments.cend())
		{
			throw CommandLine::Exception("Unknown key", _usage);
		}

		return std::any_cast<T>(it->Value);

	}

	template<typename T>
	T Value(std::string_view key, const T& defaultValue) const
	{
		const auto it = Get(key);

		if (it == _arguments.cend())
		{
			return defaultValue;
		}

		return std::any_cast<T>(it->Value);
	}

	std::string Usage() const;

private:
	std::vector<Argument>::const_iterator Get(std::string_view key) const;

	std::vector<Argument> _arguments;
	std::string _usage;
};