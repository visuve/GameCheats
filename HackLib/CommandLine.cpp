#include "CommandLine.hpp"
#include "Exceptions.hpp"

std::ostream& operator << (std::ostream& stream, const CommandLine::Argument& argument)
{
	stream << std::left << std::setw(20);

	if (argument.Type == typeid(std::nullopt))
	{
		stream << argument.Key;
	}
	else if (argument.Type == typeid(std::filesystem::path))
	{
		stream << argument.Key + "=<file>";
	}
	else if (argument.Type == typeid(double))
	{
		stream << argument.Key + "=<double>";
	}
	else if (argument.Type == typeid(float))
	{
		stream << argument.Key + "=<float>";
	}
	else if (argument.Type == typeid(int))
	{
		stream << argument.Key + "=<integer>";
	}
	else if (argument.Type == typeid(std::string))
	{
		stream << argument.Key + "=<word>";
	}
	else
	{
		throw ArgumentException("Not supported type");
	}

	return stream << argument.Description;
}

CommandLine::Exception::Exception(
	const std::string& what,
	const std::string& usage) :
	_usage(usage),
	_what("Error: " + what)
{
}

std::string_view CommandLine::Exception::Usage() const
{
	return _usage;
}

const char* CommandLine::Exception::what() const throw ()
{
	return _what.c_str();
}

CommandLine::Argument::Argument(const std::string& key, const std::type_index type, const std::string& description) :
	Key(key),
	Type(type),
	Description(description)
{
}

bool CommandLine::Argument::Parse(std::string_view value)
{
	if (!value.starts_with(Key))
	{
		return false;
	}

	if (Type == typeid(std::nullopt))
	{
		if (value == Key)
		{
			Value = true;
			return true;
		}

		// This odd case should never occurr
		return false;
	}

	value.remove_prefix(Key.size());

	if (value.empty() || value.front() != L'=')
	{
		false;
	}

	value.remove_prefix(1);

	if (Type == typeid(std::filesystem::path))
	{
		Value = std::filesystem::path(value);
	}
	else if (Type == typeid(double))
	{
		Value = std::stod(value.data());
	}
	else if (Type == typeid(float))
	{
		Value = std::stof(value.data());
	}
	else if (Type == typeid(int))
	{
		Value = std::stoi(value.data());
	}
	else if (Type == typeid(std::string))
	{
		Value = std::string(value);
	}
	else
	{
		return false;
	}

	return true;
}

CommandLine::CommandLine(const std::vector<std::string>& given, std::initializer_list<Argument> expected) :
	_arguments(expected)
{
	std::stringstream usage;

	usage << std::filesystem::path(given[0]).stem().string();
	usage << " - usage:\n\n " << given[0] << std::endl;

	for (const Argument& argument : _arguments)
	{
		usage << "  " << argument << std::endl;
	}

	size_t valid = 0;
	size_t invalid = 0;

	// Intentionally skip the first
	for (size_t i = 1; i < given.size(); ++i)
	{
		std::string kvCombo = given[i];

		auto startsWith = [&](const Argument& e)
		{
			return kvCombo.starts_with(e.Key);
		};

		auto it = std::find_if(_arguments.begin(), _arguments.end(), startsWith);

		if (it == _arguments.end())
		{
			usage << "\n  Argument \"" << kvCombo
				<< "\" is unknown." << std::endl;

			++invalid;
			continue;
		}
		
		if (it->Parse(kvCombo))
		{
			++valid;
		}
		else
		{
			usage << "\n  Argument \"" << kvCombo
				<< "\" could not be parsed." << std::endl;

			++invalid;
		}
	}

	_usage = usage.str();

	if (invalid > 0)
	{
		throw CommandLine::Exception("Invalid arguments", _usage);
	}

	if (valid == 0)
	{
		throw CommandLine::Exception("Missing arguments", _usage);
	}
}

CommandLine::CommandLine(int argc, char** argv, std::initializer_list<Argument> expected) :
	CommandLine({ argv, argv + argc }, expected)
{
}

bool CommandLine::Contains(std::string_view key) const
{
	return Get(key) != _arguments.cend();
}

std::string CommandLine::Usage() const
{
	return _usage;
}