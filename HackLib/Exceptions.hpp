#pragma once

#include <source_location>

inline std::string ExceptionMessage(std::string_view message, const std::source_location& location)
{
	return std::format("{0}:{1}: {2}", 
		std::filesystem::path(location.file_name()).filename().string(),
		location.line(), 
		message);
}

inline auto ArgumentException(
	std::string_view message, 
	const std::source_location& location = std::source_location::current())
{
	return std::invalid_argument(ExceptionMessage(message, location));
}

inline auto RangeException(
	std::string_view message, 
	const std::source_location& location = std::source_location::current())
{
	return std::range_error(ExceptionMessage(message, location));
}

inline auto RuntimeException(
	std::string_view message, 
	const std::source_location& location = std::source_location::current())
{
	return std::runtime_error(ExceptionMessage(message, location));
}

inline auto OutOfRangeException(
	std::string_view message, 
	const std::source_location& location = std::source_location::current())
{
	return std::out_of_range(ExceptionMessage(message, location));
}

inline auto LogicException(
	std::string_view message, 
	const std::source_location& location = std::source_location::current())
{
	return std::logic_error(ExceptionMessage(message, location));
}

inline auto Win32Exception(
	std::string_view message, 
	int64_t error = GetLastError(), // large enough to hold any error
	const std::source_location& location = std::source_location::current())
{
	return std::system_error(
		static_cast<int>(error), // just has to be downcasted :-D ...
		std::system_category(),
		ExceptionMessage(message, location)); 
	
	// ... this fuckery is to prevent compiler warnings
	// Thanks Micro$soft for having DWORD, HRESULT, LSTATUS, NTSTATUS etc.
	// Exceptions are different in that regard as in the end they are
	// derived from the same class i.e. std::exception. Of course throwing
	// anything is possible, but that's just pure evil.
}