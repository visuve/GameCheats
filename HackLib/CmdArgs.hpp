#pragma once

#include "NonCopyable.hpp"

class CmdArgs
{
public:
	class MissingArguments : public std::exception
	{
	public:
		MissingArguments(const std::wstring& usage);
		std::wstring_view Usage() const;
		const char* what() const throw ();
	private:
		const std::wstring _usage;
	};

	using Argument = std::tuple<std::wstring, std::wstring>;

	CmdArgs(int argc, wchar_t** argv, std::initializer_list<Argument> expected);
	NonCopyable(CmdArgs);

	bool Contains(std::wstring_view x) const;

private:
	const std::vector<Argument> _expected;
	const std::vector<std::wstring> _arguments;
};