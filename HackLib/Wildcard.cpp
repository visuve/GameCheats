#include "Wildcard.hpp"
#include "Exceptions.hpp"
#include "Strings.hpp"

Wildcard::Wildcard(std::string_view wildcard) :
	_wildcard(wildcard)
{
}

Wildcard::~Wildcard()
{
}

std::vector<size_t> Wildcard::Matches(std::string_view text) const
{
	std::vector<size_t> matchIndexes;

	if (_wildcard.empty())
	{
		return matchIndexes;
	}

	for (size_t startIdx = 0; startIdx < text.size(); ++startIdx)
	{
		size_t textIter = startIdx;
		size_t wildIter = 0;
		size_t lastMatch = std::string::npos;
		size_t star = std::string::npos;

		while (textIter < text.size() && wildIter < _wildcard.size())
		{
			if (Strings::IEquals(_wildcard[wildIter], text[textIter]) || _wildcard[wildIter] == '?')
			{
				++textIter;
				++wildIter;
			}
			else if (_wildcard[wildIter] == '*')
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
				break;
			}
		}

		while (wildIter < _wildcard.size() && _wildcard[wildIter] == '*')
		{
			++wildIter;
		}

		if (wildIter == _wildcard.size() || (wildIter == _wildcard.size() - 1 && _wildcard[wildIter] == '*'))
		{
			matchIndexes.push_back(startIdx);

			if (textIter > startIdx)
			{
				startIdx = textIter - 1;
			}
			else
			{
				startIdx = textIter;
			}
		}
	}

	return matchIndexes;
}

ByteWildcard::ByteWildcard(std::string_view wildcard) :
	Wildcard(wildcard)
{
	const auto isValidChar = [](char c)
	{
		return c == '*' || c == '?' || c == ' ' || Strings::IsHex(c);
	};

	if (!std::ranges::all_of(wildcard, isValidChar))
	{
		throw ArgumentException("Invalid character. Only uppercase hex, space, *, or ? allowed.");
	}

	std::string input(wildcard);
	std::stringstream stream(input);
	std::string raw;

	while (stream >> raw)
	{
		if (raw.length() != 2)
		{
			throw ArgumentException("Segments must be exactly 2 characters.");
		}

		_pattern.push_back(ParseByte(raw[0], raw[1]));
	}
}

std::vector<size_t> ByteWildcard::Matches(const ByteStream& bytes) const
{
	std::vector<size_t> matches;

	const size_t sSize = bytes.Size();
	const size_t pSize = _pattern.size();

	if (pSize == 0 || sSize == 0)
	{
		return matches;
	}

	for (size_t start = 0; start < sSize; ++start)
	{
		size_t sIdx = start;
		size_t pIdx = 0;

		size_t starPatternIdx = std::string::npos;
		size_t starStreamIdx = std::string::npos;
		size_t starMatchLen = 0;

		while (sIdx < sSize && pIdx < pSize)
		{
			const auto& pat = _pattern[pIdx];

			if (pat.IsVariable)
			{
				starPatternIdx = pIdx;
				starStreamIdx = sIdx;
				starMatchLen = 1;

				if (MatchStream(bytes, starStreamIdx, starMatchLen, pat))
				{
					sIdx = starStreamIdx + starMatchLen;
					++pIdx;
					continue;
				}
			}
			else if (MatchesByte(bytes[sIdx], pat))
			{
				++sIdx;
				++pIdx;
				continue;
			}

			if (starPatternIdx != std::string::npos)
			{
				const auto& starPat = _pattern[starPatternIdx];
				++starMatchLen;

				if (MatchStream(bytes, starStreamIdx, starMatchLen, starPat))
				{
					sIdx = starStreamIdx + starMatchLen;
					pIdx = starPatternIdx + 1;
					continue;
				}
			}

			break;
		}

		if (pIdx == pSize)
		{
			matches.push_back(start);
		}
	}

	return matches;
}

ByteWildcard::PatternByte ByteWildcard::ParseByte(char high, char low) const
{
	if ((high == '*' && low == '?') || (high == '?' && low == '*'))
	{
		throw ArgumentException("Invalid wildcard: '*' cannot be combined with '?' in the same byte.");
	}

	PatternByte pb;
	pb.IsVariable = (high == '*' || low == '*');

	auto [highVal, highMask] = ParseNibble(high);
	auto [lowVal, lowMask] = ParseNibble(low);

	if (pb.IsVariable)
	{
		if (high != '*')
		{
			pb.PrefixMask = HighNibbleMask;
			pb.PrefixValue = static_cast<uint8_t>(highVal << 4);
		}

		if (low != '*')
		{
			pb.SuffixMask = LowNibbleMask;
			pb.SuffixValue = lowVal;
		}
	}
	else
	{
		pb.PrefixMask = static_cast<uint8_t>((highMask << 4) | lowMask);
		pb.PrefixValue = static_cast<uint8_t>((highVal << 4) | lowVal);
	}

	return pb;
}

std::pair<uint8_t, uint8_t> ByteWildcard::ParseNibble(char c) const
{
	if (c == '?' || c == '*')
	{
		return std::make_pair<uint8_t, uint8_t>(0x00, 0x00);
	}

	return std::make_pair<uint8_t, uint8_t>(Strings::ValueFromNibble(c), uint8_t(LowNibbleMask));
}

bool ByteWildcard::MatchesByte(uint8_t byte, const PatternByte& p) const
{
	return (byte & p.PrefixMask) == p.PrefixValue;
}

bool ByteWildcard::MatchStream(const ByteStream& bytes, size_t start, size_t& len, const PatternByte& p) const
{
	const size_t size = bytes.Size();

	while (start + len <= size)
	{
		const uint8_t first = bytes[start];
		const uint8_t last = bytes[start + len - 1];

		const bool prefixOk = (first & p.PrefixMask) == p.PrefixValue;
		const bool suffixOk = (last & p.SuffixMask) == p.SuffixValue;

		if (prefixOk && suffixOk)
		{
			return true;
		}

		++len;
	}

	return false;
}
