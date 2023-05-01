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

	static std::string Prefix(const std::source_location& location);

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

	template <typename T>
	void Hex(std::string_view name, T value) = delete;

	template <>
	void Hex(std::string_view name, uint8_t value)
	{
		_stream << std::format(" {} = 0x{:02X}", name, value);
	}

	template <>
	void Hex(std::string_view name, uint16_t value)
	{
		_stream << std::format(" {} = 0x{:04X}", name, value);
	}

	template <>
	void Hex(std::string_view name, uint32_t value)
	{
		_stream << std::format(" {} = 0x{:08X}", name, value);
	}

	template <>
	void Hex(std::string_view name, uint64_t value)
	{
		_stream << std::format(" {} = 0x{:016X}", name, value);
	}

	template <typename T>
	void Hex(std::string_view name, T* value) requires std::convertible_to<T, size_t>
	{
		Hex(name, reinterpret_cast<size_t>(value));
	}

private:
	static std::mutex _mutex;
	std::ostream& _stream;
	Modifier _modifier = Modifier::Space;
	Color _color = Color::Default;
	const bool _isConsoleOutput;
};

#ifdef _DEBUG
class ScopeLogger
{
public:
	ScopeLogger(const std::source_location& location);
	~ScopeLogger();

private:
	std::source_location _sourceLocation;
	std::chrono::high_resolution_clock::time_point _start;
};

#define JoinNoExpand(A, B) A ## B
#define JoinExpand(A, B) JoinNoExpand(A, B)

#define LogDebug Logger(std::clog, std::source_location::current())
#define LogVariable(x) LogDebug.Hex(#x, x)
#define LogScope ScopeLogger JoinExpand(logger, __LINE__)(std::source_location::current())
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
#define LogVariable(x)
#define LogScope PseudoLogger()
#endif

#define Log Logger(std::cout, std::source_location::current())
#define LogWarning Logger(std::cout, std::source_location::current()) << Logger::Color::Yellow << "Warning:"
#define LogError Logger(std::cerr, std::source_location::current())