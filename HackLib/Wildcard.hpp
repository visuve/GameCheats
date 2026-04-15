#pragma once

#include "ByteStream.hpp"

class Wildcard
{
public:
	Wildcard(std::string_view wildcard);
	virtual ~Wildcard();

	std::vector<size_t> Matches(std::string_view text) const;
protected:
	const std::string _wildcard;
};

class ByteWildcard : Wildcard
{
public:
	ByteWildcard(std::string_view wildcard);
	std::vector<size_t> Matches(const ByteStream& bytes) const;

private:
	static constexpr uint8_t HighNibbleMask = 0xF0;
	static constexpr uint8_t LowNibbleMask = 0x0F;

	struct PatternByte
	{
		bool IsVariable = false;
		uint8_t PrefixMask = 0x00;
		uint8_t PrefixValue = 0x00;
		uint8_t SuffixMask = 0x00;
		uint8_t SuffixValue = 0x00;
	};

	PatternByte ParseByte(char high, char low) const;
	std::pair<uint8_t, uint8_t> ParseNibble(char c) const;
	bool MatchesByte(uint8_t byte, const PatternByte& p) const;
	bool MatchStream(const ByteStream& bytes, size_t start, size_t& len, const PatternByte& p) const;

	std::vector<PatternByte> _pattern;
};