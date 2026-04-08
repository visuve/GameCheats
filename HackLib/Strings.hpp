#pragma once

#include "Exceptions.hpp"

namespace Strings
{
	std::string ToUtf8(const std::wstring_view str);
	std::wstring ToUtf8(const std::string_view str);

	constexpr bool IsHex(char c)
	{
		const uint32_t uc = static_cast<uint32_t>(c);
		const uint32_t isNumber = (~(uc - 48u) & (uc - 58u)) >> 31;
		const uint32_t isUpperAlpha = (~(uc - 65u) & (uc - 71u)) >> 31;
		return static_cast<bool>(isNumber | isUpperAlpha);
	}

	constexpr uint8_t ValueFromNibble(char nibble)
	{
		if (!IsHex(nibble))
		{
			throw ArgumentException("Value must be 0-9, A-F");
		}

		const uint8_t un = static_cast<uint8_t>(nibble);
		return (un & 0x0Fu) + (un >> 6u) * 9u;
	}

	constexpr char NibbleFromValue(uint8_t value)
	{
		const uint8_t v = value & 0x0Fu;
		return 0x30u + v + (((v + 6u) >> 4u) * 7u);
	}

	constexpr uint8_t ValueFromNibbles(std::pair<char, char> nibbles)
	{
		return ValueFromNibble(nibbles.first) << 4 | ValueFromNibble(nibbles.second) << 0;
	}

	constexpr std::pair<char, char> NibblesFromValue(uint8_t byte)
	{
		return { NibbleFromValue(byte >> 4), NibbleFromValue(byte) };
	}

	std::string ToHex(std::span<const uint8_t> data, char delimiter = 0);

	bool IEquals(char a, char b);
	bool IEquals(std::string_view a, std::string_view b);
}