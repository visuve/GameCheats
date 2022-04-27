#include "Logger.hpp"

#include <mutex>

std::mutex LogMutex;

Logger::Logger(std::ostream& stream, const std::source_location& location) :
	_stream(stream)
{
	LogMutex.lock();

	if (&_stream == &std::cout)
	{
		_stream << "\033[92m\033[40m"; // light green, black background
	}

	if (&_stream == &std::cerr)
	{
		_stream << "\033[91m\033[40m"; // light red, black background
	}

	const std::chrono::zoned_time currentTime(
		std::chrono::current_zone(), 
		std::chrono::system_clock::now());

	const std::string fileName = std::filesystem::path(
		location.file_name()).filename().string();

	_stream << std::format(
		"[{:%T}][{}:{}]",
		currentTime,
		fileName,
		location.line());
}

Logger::~Logger()
{
	_stream << "\033[0m" << std::endl;

	LogMutex.unlock();
}

Logger& Logger::operator << (Modifier x)
{
	_modifier = x;
	return *this;
}
