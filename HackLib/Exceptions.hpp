#pragma once

#include <format>
#include <stdexcept>
#include <system_error>

#define ExceptionMessage(x) std::format("{}:{}: {}", __FILE__, __LINE__, x)
#define RangeException(x) std::range_error(ExceptionMessage(x))
#define LogicException(x) std::logic_error(ExceptionMessage(x))
#define Win32Exception(x) std::system_error(GetLastError(), std::system_category(), ExceptionMessage(x))