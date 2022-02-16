#pragma once

#include <stdexcept>
#include <system_error>

#define STRINGIFY2(x) #x
#define STRINGIFY1(x) STRINGIFY2(x)
#define LINE_STRING STRINGIFY1(__LINE__)

#define ExceptionMessage(msg) __FILE__ ":" LINE_STRING ": " msg
#define RangeException(msg) std::range_error(ExceptionMessage(msg))
#define LogicException(msg) std::logic_error(ExceptionMessage(msg))
#define Win32Exception(msg) std::system_error(GetLastError(), std::system_category(), ExceptionMessage(msg))
#define Win32ExceptionEx(msg, code) std::system_error(code, std::system_category(), ExceptionMessage(msg))