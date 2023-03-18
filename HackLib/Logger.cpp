#include "Logger.hpp"

std::mutex Logger::_mutex;

Logger::Logger(std::ostream& stream, const std::source_location& location) :
	_stream(stream),
	_isConsoleOutput(&stream == &std::cout || &stream == &std::cerr || &stream == &std::clog)
{
	if (!_stream)
	{
		return;
	}

	_mutex.lock();

	if (&_stream == &std::cout)
	{
		_stream << Background(Color::Black) << Foreground(Color::LightGreen);
	}
	else if (&_stream == &std::cerr)
	{
		_stream << Background(Color::Black) <<  Foreground(Color::Red);
	}
	else if (&_stream == &std::clog)
	{
		_stream << Background(Color::Black) << Foreground(Color::DarkGray);
	}

	_stream << Prefix(location);
}

Logger::~Logger()
{
	if (!_stream)
	{
		return;
	}

	if (_isConsoleOutput)
	{
		_stream << Foreground(Color::Default) << std::endl;
	}

	_mutex.unlock();
}

Logger& Logger::operator << (Modifier x)
{
	_modifier = x;
	return *this;
}

Logger& Logger::operator << (Color x)
{
	_color = x;
	return *this;
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
	if (std::clog)
	{
		std::clog << Logger::Background(Logger::Color::Black)
			<< Logger::Foreground(Logger::Color::Magenta)
			<< Logger::Prefix(_sourceLocation) 
			<< " Enter" << std::endl;
	}

	_start = std::chrono::high_resolution_clock::now();
}

ScopeLogger::~ScopeLogger()
{
	if (std::clog)
	{
		auto stop = std::chrono::high_resolution_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop - _start);

		std::clog << Logger::Background(Logger::Color::Black)
			<< Logger::Foreground(Logger::Color::Magenta)
			<< Logger::Prefix(_sourceLocation)
			<< " Exit (~" << diff << ')'
			<< std::endl;
	}
}
#endif