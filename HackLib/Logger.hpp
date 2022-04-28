#pragma once

#include "NonCopyable.hpp"

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
		switch (_color)
		{
			case Color::Black:
			case Color::Default:
				break;
			default:
				_stream << _foregrounds.at(Color::Default) << _foregrounds.at(_color);
				break;
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
	std::ostream& _stream;
	Modifier _modifier = Modifier::Space;
	Color _color = Color::Default;

	const static std::map<Color, const std::string> _foregrounds;
	const static std::map<Color, const std::string> _backgrounds;
};

#define Log Logger(std::cout, std::source_location::current())
#define LogError Logger(std::cerr, std::source_location::current())