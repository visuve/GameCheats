#include "../Mega.pch"

CmdArgs::CmdArgs(int argc, wchar_t** argv, std::initializer_list<Argument> expected) :
	_arguments(argv, argv + argc),
	_expected(expected)
{
}

CmdArgs::~CmdArgs()
{
	if (!Ok())
	{
		Usage();
	}
}

bool CmdArgs::Contains(std::wstring_view x) const
{
	const auto equals = [&](const std::wstring& argument)
	{
		return argument == x;
	};

	return std::any_of(_arguments.cbegin(), _arguments.cend(), equals);
}

bool CmdArgs::Ok() const
{
	if (_ok.has_value())
	{
		return _ok.value();
	}

	for (const Argument& expected : _expected)
	{
		if (!Contains(std::get<0>(expected)))
		{
			_ok = false;
			return false;
		}
	}

	_ok = true;
	return true;
}

void CmdArgs::Usage() const
{
	std::wcout << L"Usage:\n\n " << _arguments[0] << std::endl;
		
	for (const Argument& expected : _expected)
	{
		std::apply([](auto&&... x) 
		{
			std::wcout << L"  ";
			((std::wcout << std::left << std::setw(15) << x), ...);
			std::wcout << std::endl;
		}, expected);
	}
}