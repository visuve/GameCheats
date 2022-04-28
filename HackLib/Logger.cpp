#include "Logger.hpp"

#include <mutex>

std::mutex LogMutex;

Logger::Logger(std::ostream& stream, const std::source_location& location) :
	_stream(stream)
{
	if (!_stream)
	{
		return;
	}

	LogMutex.lock();

	_stream << _backgrounds.at(Color::Black);

	if (&_stream == &std::cout)
	{
		_stream << _foregrounds.at(Color::LightGreen);
	}

	if (&_stream == &std::cerr)
	{
		_stream << _foregrounds.at(Color::LightRed);
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

	_stream << _foregrounds.at(Color::Default) << std::endl;

	LogMutex.unlock();
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

const std::map<Logger::Color, const std::string> Logger::_foregrounds =
{
	{ Color::Black, "\033[30m" },
	{ Color::Red, "\033[31m" },
	{ Color::Green, "\033[32m" },
	{ Color::Yellow, "\033[33m" },
	{ Color::Blue, "\033[34m" },
	{ Color::Magenta, "\033[35m" },
	{ Color::Cyan, "\033[36m" },
	{ Color::LightGray, "\033[37m" },
	{ Color::DarkGray, "\033[90m" },
	{ Color::LightRed, "\033[91m" },
	{ Color::LightGreen, "\033[92m" },
	{ Color::LightYellow, "\033[93m" },
	{ Color::LightBlue, "\033[94m" },
	{ Color::LightMagenta, "\033[95m" },
	{ Color::LightCyan, "\033[96m" },
	{ Color::White, "\033[97m" },
	{ Color::Default, "\033[0m" }
};

const std::map<Logger::Color, const std::string> Logger::_backgrounds =
{
	{ Color::Black, "\033[40m" },
	{ Color::Red, "\033[41m" },
	{ Color::Green, "\033[42m" },
	{ Color::Yellow, "\033[43m" },
	{ Color::Blue, "\033[44m" },
	{ Color::Magenta, "\033[45m" },
	{ Color::Cyan, "\033[46m" },
	{ Color::LightGray, "\033[47m" },
	{ Color::DarkGray, "\033[100m" },
	{ Color::LightRed, "\033[101m" },
	{ Color::LightGreen, "\033[102m" },
	{ Color::LightYellow, "\033[103m" },
	{ Color::LightBlue, "\033[104m" },
	{ Color::LightMagenta, "\033[105m" },
	{ Color::LightCyan, "\033[106m" },
	{ Color::White, "\033[107m" }
};
