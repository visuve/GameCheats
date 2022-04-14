#pragma once

#include "NonCopyable.hpp"

class CmdArgs
{
public:
	using Argument = std::tuple<std::wstring, std::wstring>;

	CmdArgs(int argc, wchar_t** argv, std::initializer_list<Argument> expected);
	NonCopyable(CmdArgs);
	~CmdArgs();

	bool Contains(std::wstring_view x) const;
	bool Ok() const;
	void Usage() const;

private:
	const std::vector<Argument> _expected;
	const std::vector<std::wstring> _arguments;
	mutable std::optional<bool> _ok;
};