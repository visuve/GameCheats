#include "Logger.hpp"

std::mutex Logger::_mutex;

Logger::Logger(std::ostream& stream, const std::source_location& location) :
	_stream(stream)
{
	if (!_stream)
	{
		return;
	}

	_mutex.lock();

	_stream << Background(Color::Black);

	if (&_stream == &std::cout)
	{
		_stream << Foreground(Color::LightGreen);
	}

	if (&_stream == &std::cerr)
	{
		_stream << Foreground(Color::Red);
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

	_stream << Foreground(Color::Default) << std::endl;

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