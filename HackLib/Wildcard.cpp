#include "Wildcard.hpp"
#include "Exceptions.hpp"
#include "Strings.hpp"

constexpr bool IsAcceptable(char c)
{
	switch (c)
	{
		case '*': return true;
		case '?': return true;
	}

	return Strings::IsAlphaNumeric(c);
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
			(Strings::IEquals(_wildcard[wildIter], text[textIter]) || _wildcard[wildIter] == '?'))
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