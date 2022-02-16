#pragma once

#include "NonCopyable.hpp"
#include "Exceptions.hpp"

#include <filesystem>

class Registry
{
public:
	Registry(HKEY parent, std::wstring_view subKey, REGSAM access = KEY_ALL_ACCESS);
	NonCopyable(Registry);
	virtual ~Registry();

	bool IsPredefined() const;

	template <typename T>
	T Read(std::wstring_view valueName) const = delete;

	template <>
	std::wstring Read(std::wstring_view valueName) const
	{
		DWORD valueSize = 0;

		LRESULT result = RegGetValueW(
			_key,
			nullptr,
			valueName.data(),
			RRF_RT_REG_SZ,
			nullptr,
			nullptr,
			&valueSize);

		if (result != ERROR_SUCCESS)
		{
			throw Win32ExceptionEx("RegGetValueW", result);
		}

		std::wstring value(valueSize / sizeof(wchar_t), 0);

		result = RegGetValueW(
			_key,
			nullptr,
			valueName.data(),
			RRF_RT_REG_SZ,
			nullptr,
			&value.front(),
			&valueSize);

		if (result != ERROR_SUCCESS)
		{
			throw Win32ExceptionEx("RegGetValueW", result);
		}

		value.resize(valueSize / sizeof(wchar_t) - 1);

		return value;
	}

	template <>
	std::filesystem::path Read(std::wstring_view valueName) const
	{
		return Read<std::wstring>(valueName);
	}

	template <>
	DWORD Read(std::wstring_view valueName) const
	{
		DWORD value = 0;
		DWORD valueSize = sizeof(DWORD);

		LRESULT result = RegGetValueW(
			_key,
			nullptr,
			valueName.data(),
			RRF_RT_REG_DWORD,
			nullptr,
			&value,
			&valueSize);

		if (result != ERROR_SUCCESS)
		{
			throw Win32ExceptionEx("RegGetValueW", result);
		}

		_ASSERT_EXPR(valueSize == sizeof(DWORD), L"RegGetValueW size mismatch!");

		return value;
	}

	template <typename T>
	void Write(std::wstring_view valueName, T value) const = delete;

	template <>
	void Write(std::wstring_view valueName, std::wstring_view value) const
	{
		// Includes null terminator
		DWORD valueSize = value.size() * sizeof(wchar_t) + sizeof(wchar_t);

		LONG result = RegSetValueExW(
			_key,
			valueName.data(),
			0,
			REG_SZ,
			reinterpret_cast<const BYTE*>(value.data()),
			valueSize);

		if (result != ERROR_SUCCESS)
		{
			throw Win32ExceptionEx("RegSetValueExW", result);
		}
	}

	template <>
	void Write(std::wstring_view valueName, DWORD value) const
	{
		LONG result = ::RegSetValueExW(
			_key,
			valueName.data(),
			0,
			REG_DWORD,
			reinterpret_cast<const BYTE*>(&value),
			sizeof(DWORD));

		if (result != ERROR_SUCCESS)
		{
			throw Win32ExceptionEx("RegSetValueExW", result);
		}
	}

private:
	HKEY _key = nullptr;
};