#pragma once

class CmdArgs
{
public:
	inline CmdArgs(int argc, wchar_t** argv) :
		_arguments(argv, argv + argc)
	{
	}

	inline bool Contains(std::wstring_view x) const
	{
		const auto equals = [&](const std::wstring& argument)
		{
			return argument == x;
		};

		return std::any_of(_arguments.cbegin(), _arguments.cend(), equals);
	}

private:
	const std::vector<std::wstring> _arguments;
};