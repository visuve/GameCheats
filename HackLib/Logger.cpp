#include "Logger.hpp"

std::mutex LogMutex;

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

std::string Logger::Prefix(const std::source_location& location)
{
	const std::chrono::zoned_time currentTime(
	std::chrono::current_zone(),
	std::chrono::system_clock::now());

	const std::string fileName = std::filesystem::path(
		location.file_name()).filename().string();

	return std::format("[{:%T}][{}:{}]", currentTime, fileName, location.line());
}

#ifdef _DEBUG
ScopeLogger::ScopeLogger(const std::source_location& location) :
	_sourceLocation(location)
{
	if (std::cout)
	{
		LogMutex.lock();

		std::cout << Logger::Background(Logger::Color::Black)
			<< Logger::Foreground(Logger::Color::Magenta)
			<< Logger::Prefix(_sourceLocation)
			<< " Enter\n";

		LogMutex.unlock();

		_start = std::chrono::high_resolution_clock::now();
	}
}

ScopeLogger::~ScopeLogger()
{
	if (std::cout)
	{
		auto stop = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop - _start);

		LogMutex.lock();

		std::cout << Logger::Background(Logger::Color::Black)
			<< Logger::Foreground(Logger::Color::Magenta)
			<< Logger::Prefix(_sourceLocation)
			<< " Exit (~" << diff << ")\n";

		LogMutex.unlock();
	}
}
#endif