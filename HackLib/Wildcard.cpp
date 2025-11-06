#include "Wildcard.hpp"
#include "Exceptions.hpp"
#include "StrConvert.hpp"

constexpr bool IsWildcard(char c)
{
	return c == '*' || c == '?';
}

Wildcard::Wildcard(std::string_view wildcard) :
	_wildcard(wildcard)
{
}

Wildcard::~Wildcard()
{
}

bool Wildcard::Matches(std::string_view text)
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
	// TODO: this is mostly copy-paste from ByteStream class.
	// Find "common nominators" and reduce
	std::stringstream stream;
	stream << wildcard;

	std::string raw;

	constexpr auto unacceptable = [](char c)->bool
	{
		return !(IsWildcard(c) || std::isxdigit(c));
	};

	while (std::getline(stream, raw, ' '))
	{
		if (raw.length() != 2)
		{
			throw ArgumentException(
				"Arguments have to be between 00-FF (zero padded) and separated with space");
		}

		if (std::any_of(raw.cbegin(), raw.cend(), unacceptable))
		{
			throw ArgumentException("Only ?, * and hexadecimal characters accepted");
		}
	}
}

bool ByteWildcard::Matches(ByteStream bytes)
{
	// TODO: this ain't a thing of beauty, but will do for now
	// Maybe add similar std::formatter as in the Pointer class
	std::stringstream ss;
	ss << bytes;
	return Wildcard::Matches(ss.str());
}
