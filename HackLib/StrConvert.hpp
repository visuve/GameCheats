#pragma once

namespace StrConvert
{
	std::string ToUtf8(const std::wstring_view str);
	std::wstring ToUtf8(const std::string_view str);

	bool IEquals(char a, char b);
	bool IEquals(std::string_view a, std::string_view b);
}