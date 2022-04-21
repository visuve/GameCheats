#pragma once

#include <source_location>

inline std::string ExceptionMessage(
	std::string_view message,
	const std::source_location& location = std::source_location::current())
{
	return std::format("{0}:{1}: {2}", 
		std::filesystem::path(location.file_name()).filename().string(),
		location.line(), 
		message);
}

#define ArgumentException(msg) std::invalid_argument(ExceptionMessage(msg))
#define RangeException(msg) std::range_error(ExceptionMessage(msg))
#define RuntimeException(msg) std::runtime_error(ExceptionMessage(msg))
#define OutOfRangeException(msg) std::out_of_range(ExceptionMessage(msg))
#define LogicException(msg) std::logic_error(ExceptionMessage(msg))
#define Win32Exception(msg) std::system_error(GetLastError(), std::system_category(), ExceptionMessage(msg))
#define Win32ExceptionEx(msg, code) std::system_error(code, std::system_category(), ExceptionMessage(msg))