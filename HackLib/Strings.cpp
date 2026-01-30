#include "Strings.hpp"

std::string Strings::ToUtf8(const std::wstring_view str)
{
	int required = WideCharToMultiByte(
		CP_UTF8,
		0,
		str.data(),
		static_cast<int>(str.size()),
		nullptr,
		0,
		nullptr,
		nullptr);

	if (required < 0)
	{
		throw ArgumentException("WideCharToMultiByte");
	}

	std::string result(required, 0);

	required = WideCharToMultiByte(
		CP_UTF8,
		0,
		str.data(),
		static_cast<int>(str.size()),
		result.data(),
		required,
		nullptr,
		nullptr);

	_ASSERT(result.size() == static_cast<size_t>(required));

	return result;
}

std::wstring Strings::ToUtf8(const std::string_view str)
{
	int required = MultiByteToWideChar(
		CP_UTF8,
		0,
		str.data(),
		static_cast<int>(str.size()),
		nullptr,
		0);

	if (required < 0)
	{
		throw ArgumentException("WideCharToMultiByte");
	}

	std::wstring result(required, 0);

	required = MultiByteToWideChar(
		CP_UTF8,
		0,
		str.data(),
		static_cast<int>(str.size()),
		result.data(),
		required);

	_ASSERT(result.size() == static_cast<size_t>(required));

	return result;
}

std::string Strings::ToHex(std::span<const uint8_t> data, char delimiter)
{
	if (data.empty())
	{
		throw ArgumentException("The data is empty");
	}

	const size_t required = (data.size() * 2) + (delimiter ? data.size() - 1 : 0);
	std::string result(required, delimiter);

	std::string::iterator it = result.begin();

	for (uint8_t byte : data)
	{
		auto [high, low] = NibblesFromValue(byte);
		*it++ = high;
		*it++ = low;

		if (delimiter && it != result.end())
		{
			++it; // Skip the delimiter
		}
	}

	return result;
}

bool Strings::IEquals(char a, char b)
{
	// https://en.cppreference.com/w/cpp/string/byte/tolower#Notes
	return std::tolower(static_cast<uint8_t>(a)) ==
		std::tolower(static_cast<uint8_t>(b));
}

bool Strings::IEquals(std::string_view a, std::string_view b)
{
	if (a.size() != b.size())
	{
		return false;
	}

	return _strnicmp(a.data(), b.data(), a.size()) == 0;
}
