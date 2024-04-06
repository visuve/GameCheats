#include "Logger.hpp"

namespace
{
	constexpr std::string_view Foreground(Logger::Color c)
	{
		switch (c)
		{
			case Logger::Color::Black: return "\033[30m";
			case Logger::Color::Red: return "\033[31m";
			case Logger::Color::Green: return "\033[32m";
			case Logger::Color::Yellow: return "\033[33m";
			case Logger::Color::Blue: return "\033[34m";
			case Logger::Color::Magenta: return "\033[35m";
			case Logger::Color::Cyan: return "\033[36m";
			case Logger::Color::LightGray: return "\033[37m";
			case Logger::Color::DarkGray: return "\033[90m";
			case Logger::Color::LightRed: return "\033[91m";
			case Logger::Color::LightGreen: return "\033[92m";
			case Logger::Color::LightYellow: return "\033[93m";
			case Logger::Color::LightBlue: return "\033[94m";
			case Logger::Color::LightMagenta: return "\033[95m";
			case Logger::Color::LightCyan: return "\033[96m";
			case Logger::Color::White: return "\033[97m";
			case Logger::Color::Default: return "\033[0m";
		}

		throw ArgumentException("Unknown foreground color");
	}

	constexpr std::string_view Background(Logger::Color c)
	{
		switch (c)
		{
			case Logger::Color::Black: return "\033[40m";
			case Logger::Color::Red: return "\033[41m";
			case Logger::Color::Green: return "\033[42m";
			case Logger::Color::Yellow: return "\033[43m";
			case Logger::Color::Blue: return "\033[44m";
			case Logger::Color::Magenta: return "\033[45m";
			case Logger::Color::Cyan: return "\033[46m";
			case Logger::Color::LightGray: return "\033[47m";
			case Logger::Color::DarkGray: return "\033[100m";
			case Logger::Color::LightRed: return "\033[101m";
			case Logger::Color::LightGreen: return "\033[102m";
			case Logger::Color::LightYellow: return "\033[103m";
			case Logger::Color::LightBlue: return "\033[104m";
			case Logger::Color::LightMagenta: return "\033[105m";
			case Logger::Color::LightCyan: return "\033[106m";
			case Logger::Color::White: return "\033[107m";
		}

		throw ArgumentException("Unknown background color");
	}

	std::mutex LogMutex;
}

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
	std::ostringstream(),
	_buffer(*this),
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

Logger& Logger::operator << (Color c)
{
	if (_stream && _isConsoleOutput)
	{
		_modifiers.emplace_back(Foreground(c), Foreground(_color));
	}

	return *this;
}

Logger& Logger::operator << (Modifier m)
{
	if (_stream && m == Logger::Modifier::Quoted)
	{
		_modifiers.emplace_back("\"", "\"");
	}

	return *this;
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