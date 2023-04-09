#include "CmdArgs.hpp"
#include "Exceptions.hpp"

std::ostream& operator << (std::ostream& stream, const CmdArgs::Argument& argument)
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

CmdArgs::Exception::Exception(
	const std::string& what,
	const std::string& usage) :
	_usage(usage),
	_what("Error: " + what)
{
}

std::string_view CmdArgs::Exception::Usage() const
{
	return _usage;
}

const char* CmdArgs::Exception::what() const throw ()
{
	return _what.c_str();
}

CmdArgs::CmdArgs(const std::vector<std::string>& given, std::initializer_list<Argument> expected) :
	_arguments(given),
	_expected(expected)
{
	std::stringstream usage;

	usage << std::filesystem::path(_arguments[0]).stem().string();
	usage << " - usage:\n\n " << _arguments[0] << std::endl;

	for (const Argument& argument : _expected)
	{
		usage << "  " << argument << std::endl;
	}

	_usage = usage.str();

	for (const Argument& argument : _expected)
	{
		if (Contains(argument.Key))
		{
			return;
		}
	}

	throw CmdArgs::Exception("Missing arguments", _usage);
}

CmdArgs::CmdArgs(int argc, char** argv, std::initializer_list<Argument> expected) :
	CmdArgs({ argv, argv + argc }, expected)
{
}

bool CmdArgs::Contains(std::string_view key) const
{
	const auto equals = [&](const std::string& argument)
	{
		return argument == key || argument.starts_with(std::format("{0}=", key));
	};

	return std::any_of(_arguments.cbegin(), _arguments.cend(), equals);
}

std::string CmdArgs::Usage() const
{
	return _usage;
}

std::type_index CmdArgs::TypeByKey(std::string_view key) const
{
	const auto keyEquals = [&](const Argument& argument)->bool
	{
		return argument.Key == key;
	};

	auto expectedArgument = std::find_if(_expected.cbegin(), _expected.cend(), keyEquals);

	if (expectedArgument == _expected.cend())
	{
		throw CmdArgs::Exception("Unknown key requested", _usage);
	}

	return expectedArgument->Type;
}

std::any CmdArgs::ValueByKey(std::string_view key) const
{
	std::type_index type = TypeByKey(key);

	if (type == typeid(std::nullopt))
	{
		if (!Contains(key))
		{
			throw CmdArgs::Exception("Missing value requested", _usage);
		}

		return true;
	}

	std::string value;

	for (std::string_view providedArgument : _arguments)
	{
		if (!providedArgument.starts_with(key))
		{
			continue;
		}
		
		providedArgument.remove_prefix(key.size());

		if (providedArgument.empty() || providedArgument.front() != L'=')
		{
			throw CmdArgs::Exception(
				"Arguments with values should be passed with \'=\' sign", _usage);
		}

		providedArgument.remove_prefix(1);
		value = providedArgument;
		break;
	}
	
	if (type == typeid(std::filesystem::path))
	{
		return std::filesystem::path(value);
	}
	else if (type == typeid(double))
	{
		return std::stod(value);
	}
	else if (type == typeid(float))
	{
		return std::stof(value);
	}
	else if (type == typeid(int))
	{
		return std::stoi(value);
	}
	else if (type == typeid(std::string))
	{
		return value;
	}

	throw CmdArgs::Exception("Unsupported argument type requested", _usage);
}