#include "CmdArgs.hpp"
#include "Exceptions.hpp"

std::wostream& operator << (std::wostream& stream, const CmdArgs::Argument& argument)
{
	stream << std::left << std::setw(20);

	std::type_index type = std::get<1>(argument);

	if (type == typeid(std::nullopt))
	{
		stream << std::get<0>(argument);
	}
	else if (type == typeid(std::filesystem::path))
	{
		stream << std::get<0>(argument) + L"=<file>";
	}
	else if (type == typeid(double))
	{
		stream << std::get<0>(argument) + L"=<double>";
	}
	else if (type == typeid(float))
	{
		stream << std::get<0>(argument) + L"=<float>";
	}
	else if (type == typeid(int))
	{
		stream << std::get<0>(argument) + L"=<integer>";
	}
	else
	{
		throw ArgumentException("Not supported type");
	}

	return stream << std::get<2>(argument);
}

CmdArgs::Exception::Exception(
	const std::string& what,
	const std::wstring& usage) :
	_usage(usage),
	_what("Error: " + what)
{
}

std::wstring_view CmdArgs::Exception::Usage() const
{
	return _usage;
}

const char* CmdArgs::Exception::what() const throw ()
{
	return _what.c_str();
}

CmdArgs::CmdArgs(int argc, wchar_t** argv, std::initializer_list<Argument> expected) :
	_arguments(argv, argv + argc),
	_expected(expected)
{
	std::wstringstream usage;

	usage << std::filesystem::path(_arguments[0]).stem().wstring();
	usage << L" - usage:\n\n " << _arguments[0] << std::endl;

	for (const Argument& argument : _expected)
	{
		usage << L"  " << argument << std::endl;
	}

	_usage = usage.str();

	for (const Argument& argument : _expected)
	{
		if (Contains(std::get<0>(argument)))
		{
			return;
		}
	}

	throw CmdArgs::Exception("missing arguments", _usage);
}

bool CmdArgs::Contains(std::wstring_view x) const
{
	const auto equals = [&](const std::wstring& argument)
	{
		return argument == x || argument.starts_with(std::format(L"{0}=", x));
	};

	return std::any_of(_arguments.cbegin(), _arguments.cend(), equals);
}

std::type_index CmdArgs::TypeByKey(std::wstring_view key) const
{
	const auto keyEquals = [&](const Argument& argument)->bool
	{
		return std::get<0>(argument) == key;
	};

	auto expectedArgument = std::find_if(_expected.cbegin(), _expected.cend(), keyEquals);

	if (expectedArgument == _expected.cend())
	{
		throw CmdArgs::Exception("unknown key requested", _usage);
	}

	return std::get<1>(*expectedArgument);
}

std::any CmdArgs::ValueByKey(std::wstring_view key) const
{
	std::type_index type = TypeByKey(key);

	if (type == typeid(std::nullopt))
	{
		auto result = std::find(_arguments.cbegin(), _arguments.cend(), key);

		if (result == _arguments.cend())
		{
			throw CmdArgs::Exception("missing value requested", _usage);
		}

		return true;
	}

	std::wstring value;

	for (std::wstring_view providedArgument : _arguments)
	{
		if (!providedArgument.starts_with(key))
		{
			continue;
		}
		
		providedArgument.remove_prefix(key.size());

		if (providedArgument.front() != L'=')
		{
			throw CmdArgs::Exception(
				"arguments with values should be passed with equal sign \'=\'", _usage);
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

	throw CmdArgs::Exception("unsupported argument type requested", _usage);
}