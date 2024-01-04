#include "Logger.hpp"

std::mutex LogMutex;

std::string Prefix(const std::source_location& location)
{
	std::chrono::zoned_time currentTime(
		std::chrono::current_zone(),
		std::chrono::system_clock::now());

	std::string fileName = std::filesystem::path(
		location.file_name()).filename().string();

	return std::format("[{:%T}][{}:{}]", currentTime, fileName, location.line());
}

Logger::Logger(std::ostream& stream, const std::source_location& location, Color color) :
	_stream(stream),
	_color(color),
	_isConsoleOutput(&stream == &std::cout || &stream == &std::cerr || &stream == &std::clog)
{
	if (_stream)
	{
		_buffer << Prefix(location);
	}
}

Logger::~Logger()
{
	if (_stream)
	{
		LogMutex.lock();

		if (_isConsoleOutput)
		{
			_stream << Background(Color::Black);
			_stream << Foreground(_color);
		}

		_stream << _buffer.str();

		if (_isConsoleOutput)
		{
			_stream << Foreground(Color::Default);
		}

		_stream << std::endl;

		LogMutex.unlock();
	}
}

DurationLogger::DurationLogger(std::ostream& stream, const std::source_location& location, const std::string& message) :
	Logger(stream, location, Logger::Color::Magenta),
	_start(std::chrono::high_resolution_clock::now())
{
	*this << message;
}

DurationLogger::~DurationLogger()
{
	auto stop = std::chrono::high_resolution_clock::now();
	auto delta = stop - _start;
	*this << std::format("{:%T}", delta);
}