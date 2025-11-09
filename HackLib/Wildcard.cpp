#include "Wildcard.hpp"
#include "Exceptions.hpp"
#include "StrConvert.hpp"

constexpr bool IsAcceptable(char c)
{
	switch (c)
	{
		case '*': return true;

		case '0': return true;
		case '1': return true;
		case '2': return true;
		case '3': return true;
		case '4': return true;
		case '5': return true;
		case '6': return true;
		case '7': return true;
		case '8': return true;
		case '9': return true;

		case '?': return true;

		case 'A': return true;
		case 'B': return true;
		case 'C': return true;
		case 'D': return true;
		case 'E': return true;
		case 'F': return true;

		case 'a': return true;
		case 'b': return true;
		case 'c': return true;
		case 'd': return true;
		case 'e': return true;
		case 'f': return true;
	}

	return false;
}

Wildcard::Wildcard(std::string_view wildcard) :
	_wildcard(wildcard)
{
}

Wildcard::~Wildcard()
{
}

bool Wildcard::Matches(std::string_view text) const
{
	size_t textIter = 0, wildIter = 0;
	size_t lastMatch = std::string::npos, star = std::string::npos;

	while (textIter < text.size()) {

		if (wildIter < _wildcard.size() && 
			(StrConvert::IEquals(_wildcard[wildIter], text[textIter]) || _wildcard[wildIter] == '?'))
		{
			++textIter;
			++wildIter;
		}
		else if (wildIter < _wildcard.size() && _wildcard[wildIter] == '*')
		{
			star = wildIter;
			lastMatch = textIter;
			++wildIter;
		}
		else if (star != std::string::npos)
		{
			++lastMatch;
			textIter = lastMatch;
			wildIter = star + 1;
		}
		else
		{
			return false;
		}
	}

	return _wildcard.substr(wildIter) == std::string(_wildcard.size() - wildIter, '*');
}

ByteWildcard::ByteWildcard(std::string_view wildcard) :
	Wildcard(wildcard)
{
	for (const auto& subrange : std::views::split(wildcard, ' '))
	{
		if (subrange.size() != 2)
		{
			throw ArgumentException(
				"Arguments have to be between 00-FF (zero padded) and separated with space");
		}

		if (!IsAcceptable(subrange[0]) || !IsAcceptable(subrange[1]))
		{
			throw ArgumentException("Only ?, * and hexadecimal characters accepted");
		}
	}
}

bool ByteWildcard::Matches(ByteStream bytes)
{
	return Wildcard::Matches(std::string(bytes));
}