#include "StrConvert.hpp"
#include "../Mega.pch"

std::string StrConvert::ToUtf8(const std::wstring_view str)
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

	_ASSERT(result.size() == required);

	return result;
}

std::wstring StrConvert::ToUtf8(const std::string_view str)
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

	_ASSERT(result.size() == required);

	return result;
}
