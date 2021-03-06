#pragma once

#include "NonCopyable.hpp"
#include "Exceptions.hpp"

class Logger
{
public:
	enum class Color
	{
		Black = 0,
		Red,
		Green,
		Yellow,
		Blue,
		Magenta,
		Cyan,
		LightGray,
		DarkGray,
		LightRed,
		LightGreen,
		LightYellow,
		LightBlue,
		LightMagenta,
		LightCyan,
		White,
		Default
	};

	enum class Modifier
	{
		Space = ' ',
		Quoted = '"'
	};

	Logger(std::ostream& stream, const std::source_location& location);
	~Logger();

	NonCopyable(Logger)

	template <typename T>
	Logger& operator << (T x)
	{
		if (!_stream)
		{
			return *this;
		}

		if (_isConsoleOutput)
		{
			switch (_color)
			{
				case Color::Black:
				case Color::Default:
					break;
				default:
					_stream << Foreground(Color::Default) << Foreground(_color);
					break;
			}
		}

		switch (_modifier)
		{
			case Modifier::Quoted:
				_stream << " \"" << x << '"';
				break;
			default:
				_stream << ' ' << x;
				break;
		}


		_color = Color::Default;
		_modifier = Modifier::Space;
		return *this;
	}

	Logger& operator << (Modifier x);
	Logger& operator << (Color x);

private:
	static std::mutex _mutex;
	std::ostream& _stream;
	Modifier _modifier = Modifier::Space;
	Color _color = Color::Default;
	const bool _isConsoleOutput;

	static constexpr std::string_view Foreground(Color c)
	{
		switch (c)
		{
			case Color::Black: return "\033[30m";
			case Color::Red: return "\033[31m";
			case Color::Green: return "\033[32m";
			case Color::Yellow: return "\033[33m";
			case Color::Blue: return "\033[34m";
			case Color::Magenta: return "\033[35m";
			case Color::Cyan: return "\033[36m";
			case Color::LightGray: return "\033[37m";
			case Color::DarkGray: return "\033[90m";
			case Color::LightRed: return "\033[91m";
			case Color::LightGreen: return "\033[92m";
			case Color::LightYellow: return "\033[93m";
			case Color::LightBlue: return "\033[94m";
			case Color::LightMagenta: return "\033[95m";
			case Color::LightCyan: return "\033[96m";
			case Color::White: return "\033[97m";
			case Color::Default: return "\033[0m";
		}

		throw ArgumentException("Unknown foreground color");
	}

	static constexpr std::string_view Background(Color c)
	{
		switch (c)
		{
			case Color::Black: return "\033[40m";
			case Color::Red: return "\033[41m";
			case Color::Green: return "\033[42m";
			case Color::Yellow: return "\033[43m";
			case Color::Blue: return "\033[44m";
			case Color::Magenta: return "\033[45m";
			case Color::Cyan: return "\033[46m";
			case Color::LightGray: return "\033[47m";
			case Color::DarkGray: return "\033[100m";
			case Color::LightRed: return "\033[101m";
			case Color::LightGreen: return "\033[102m";
			case Color::LightYellow: return "\033[103m";
			case Color::LightBlue: return "\033[104m";
			case Color::LightMagenta: return "\033[105m";
			case Color::LightCyan: return "\033[106m";
			case Color::White: return "\033[107m";
		}

		throw ArgumentException("Unknown background color");
	}
};

#ifdef _DEBUG
#define LogDebug Logger(std::clog, std::source_location::current())
#else
struct PseudoLogger
{
	template <typename T>
	PseudoLogger& operator << (T)
	{
		return *this;
	}
};

#define LogDebug PseudoLogger()
#endif

#define Log Logger(std::cout, std::source_location::current())
#define LogError Logger(std::cerr, std::source_location::current())