#pragma once

#define ExceptionMessage(msg) \
	std::filesystem::path(__FILE__).filename().string() + \
	std::string(":") + \
	std::to_string(__LINE__) + \
	std::string(": ") + \
	std::string(msg)

#define ArgumentException(msg) std::invalid_argument(ExceptionMessage(msg))
#define RangeException(msg) std::range_error(ExceptionMessage(msg))
#define RuntimeException(msg) std::runtime_error(ExceptionMessage(msg))
#define OutOfRangeException(msg) std::out_of_range(ExceptionMessage(msg))
#define LogicException(msg) std::logic_error(ExceptionMessage(msg))
#define Win32Exception(msg) std::system_error(GetLastError(), std::system_category(), ExceptionMessage(msg))
#define Win32ExceptionEx(msg, code) std::system_error(code, std::system_category(), ExceptionMessage(msg))