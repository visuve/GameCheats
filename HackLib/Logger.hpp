#pragma once

#include "NonCopyable.hpp"

class Logger
{
public:
	enum Modifier
	{
		Space = ' ',
		Quoted = '"'
	};

	Logger(std::ostream* stream, const std::source_location& location);
	~Logger();

	NonCopyable(Logger)

	template <typename T>
	Logger& operator << (T x)
	{
		if (!_stream)
		{
			return *this;
		}

		switch (_modifier)
		{
			case Quoted:
				*_stream << " \"" << x << '"';
				_modifier = Space;
				break;
			default:
				*_stream << ' ' << x;
				break;
		}

		
		return *this;
	}

	Logger& operator << (Modifier x);

private:
	std::ostream* _stream;
	Modifier _modifier = Space;
};

#define Log Logger(&std::cout, std::source_location::current())
#define LogError Logger(&std::cerr, std::source_location::current())