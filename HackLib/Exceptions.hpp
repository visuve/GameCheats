#pragma once

struct WindowsErrorCategory : public std::error_category
{
	const char* name() const noexcept override;
	std::string message(DWORD error) const;
	std::string message(int error) const override;
};

std::string ExceptionMessage(std::string_view message, const std::source_location& location);

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
		static_cast<int>(error), // ... just has to be downcasted :-D
		WindowsErrorCategory(),
		std::format("{}:{}: {} failed! Error code 0x{:08X}",
			std::filesystem::path(location.file_name()).filename().string(),
			location.line(),
			message,
			error));
}