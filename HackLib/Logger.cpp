#include "Logger.hpp"

Logger::Logger(std::ostream& stream, const std::source_location& location) :
	_stream(stream)
{
	if (&_stream == &std::cout)
	{
		_stream << "\033[92m\033[40m"; // light green, black background
	}

	if (&_stream == &std::cerr)
	{
		_stream << "\033[91m\033[40m"; // light red, black background
	}

	_stream << std::format("[{:%T}][{}:{}]",
		std::chrono::system_clock::now(),
		std::filesystem::path(location.file_name()).filename().string(),
		location.line());
}

Logger::~Logger()
{
	_stream << "\033[0m" << std::endl;
}

Logger& Logger::operator << (Modifier x)
{
	_modifier = x;
	return *this;
}
