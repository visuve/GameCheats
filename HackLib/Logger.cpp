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
