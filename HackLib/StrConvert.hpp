#pragma once

#include "Exceptions.hpp"

namespace StrConvert
{
	std::string ToUtf8(const std::wstring_view str);
	std::wstring ToUtf8(const std::string_view str);
}