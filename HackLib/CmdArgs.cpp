#include "CmdArgs.hpp"
#include "../Mega.pch"

CmdArgs::MissingArguments::MissingArguments(const std::wstring& usage) :
	_usage(usage)
{
}

std::wstring_view CmdArgs::MissingArguments::Usage() const
{
	return _usage;
}

const char* CmdArgs::MissingArguments::what() const throw ()
{
	return "Error, missing command line arguments";
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
		std::apply([&](auto&&... x)
		{
			usage << L"  ";
			((usage << std::left << std::setw(15) << x), ...);
			usage << std::endl;
		}, argument);
	}

	for (const Argument& argument : _expected)
	{
		if (Contains(std::get<0>(argument)))
		{
			return;
		}
	}

	throw MissingArguments(usage.str());
}

bool CmdArgs::Contains(std::wstring_view x) const
{
	const auto equals = [&](const std::wstring& argument)
	{
		return argument == x;
	};

	return std::any_of(_arguments.cbegin(), _arguments.cend(), equals);
}