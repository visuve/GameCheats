#pragma once

namespace Strings
{
	std::string ToUtf8(const std::wstring_view str);
	std::wstring ToUtf8(const std::string_view str);

	constexpr bool IsAlphaNumeric(char c)
	{
		switch (c)
		{
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

	bool IEquals(char a, char b);
	bool IEquals(std::string_view a, std::string_view b);
}