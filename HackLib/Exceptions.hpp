#pragma once

#include <format>
#include <stdexcept>
#include <system_error>

#define ExceptionMessage(msg) std::format("{}:{}: {}", __FILE__, __LINE__, msg)
#define RangeException(msg) std::range_error(ExceptionMessage(msg))
#define LogicException(msg) std::logic_error(ExceptionMessage(msg))
#define Win32Exception(msg) std::system_error(GetLastError(), std::system_category(), ExceptionMessage(msg))
#define Win32ExceptionEx(msg, code) std::system_error(code, std::system_category(), ExceptionMessage(msg))