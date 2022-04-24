#pragma once

#include "NonCopyable.hpp"
#include "Exceptions.hpp"

#include <typeindex>
#include <variant>

class CmdArgs
{
public:
	class Exception : public std::exception
	{
	public:
		Exception(const std::string& what, const std::wstring& usage);
		std::wstring_view Usage() const;
		const char* what() const throw ();

	private:
		const std::wstring _usage;
		const std::string _what;
	};

	using Argument = std::tuple<std::wstring, std::type_index, std::wstring>;

	CmdArgs(int argc, wchar_t** argv, std::initializer_list<Argument> expected);
	NonCopyable(CmdArgs);

	bool Contains(std::wstring_view x) const;

	template<typename T>
	T Get(std::wstring_view key) const
	{
		return std::any_cast<T>(ValueByKey(key));
	}

	std::wstring Usage() const;

private:
	std::type_index TypeByKey(std::wstring_view key) const;
	std::any ValueByKey(std::wstring_view key) const;

	std::wstring _usage;
	const std::vector<Argument> _expected;
	const std::vector<std::wstring> _arguments;
};