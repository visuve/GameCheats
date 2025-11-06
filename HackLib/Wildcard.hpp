#pragma once

#include "ByteStream.hpp"

class Wildcard
{
public:
	Wildcard(std::string_view wildcard);
	virtual ~Wildcard();

	bool Matches(std::string_view text);
protected:
	const std::string _wildcard;
};

class ByteWildcard : Wildcard
{
public:
	ByteWildcard(std::string_view wildcard);
	bool Matches(ByteStream bytes);
};