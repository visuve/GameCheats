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
		Spaced = ' ', // Default
		Quoted = '"'
	};

	Logger(std::ostream& stream, const std::source_location& location, Color color);
	virtual ~Logger();

	NonCopyable(Logger);

	Logger& operator << (Color c)
	{
		if (_stream && _isConsoleOutput)
		{
			_modifiers.emplace_back(Foreground(c), Foreground(_color));
		}

		return *this;
	}

	Logger& operator << (Modifier m)
	{
		if (_stream && m == Logger::Modifier::Quoted)
		{
			_modifiers.emplace_back("\"", "\"");
		}

		return *this;
	}

	Logger& operator << (auto x)
	{
		if (_stream)
		{
			_buffer << ' ';

			for (auto tag = _modifiers.cbegin(); tag != _modifiers.cend(); ++tag)
			{
				_buffer << tag->first;
			}

			_buffer << x;

			for (auto tag = _modifiers.crbegin(); tag != _modifiers.crend(); ++tag)
			{
				_buffer << tag->second;
			}

			_modifiers.clear();
		}

		return *this;
	}

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
	constexpr void Plain(std::string_view name, T value)
	{
		// std::to_string limits to basic numerals
		_buffer << ' ' << name << '=' << std::to_string(value);
	}

	template <typename T>
	void Hex(std::string_view name, T value) = delete;

	template <>
	void Hex(std::string_view name, uint8_t value)
	{
		_buffer << std::format(" {} = 0x{:02X}", name, value);
	}

	template <>
	void Hex(std::string_view name, uint16_t value)
	{
		_buffer << std::format(" {} = 0x{:04X}", name, value);
	}

	template <>
	void Hex(std::string_view name, uint32_t value)
	{
		_buffer << std::format(" {} = 0x{:08X}", name, value);
	}

	template <>
	void Hex(std::string_view name, uint64_t value)
	{
		_buffer << std::format(" {} = 0x{:016X}", name, value);
	}

	template <typename T>
	constexpr void Hex(std::string_view name, T* value) requires std::convertible_to<T, size_t>
	{
		Hex(name, reinterpret_cast<size_t>(value));
	}

private:
	std::stringstream _buffer;
	std::ostream& _stream;
	const Color _color;
	std::vector<std::pair<std::string, std::string>> _modifiers;
	const bool _isConsoleOutput;
};

class DurationLogger : private Logger
{
public:
	DurationLogger(std::ostream& stream, const std::source_location& location, const std::string& message);
	~DurationLogger();

private:
	std::string _message;
	std::chrono::high_resolution_clock::time_point _start;
};

#ifdef _DEBUG
#define LogDebug Logger(std::cout, std::source_location::current(), Logger::Color::DarkGray)
#define LogVariable(x) LogDebug.Plain(#x, x)
#define LogVariableHex(x) LogDebug.Hex(#x, x)
#else
struct PseudoLogger
{
	template <typename T>
	constexpr PseudoLogger& operator << (T)
	{
		return *this;
	}
};
#define LogDebug PseudoLogger()
#define LogVariable(x)
#define LogVariableHex(x)
#endif

#define JoinNoExpand(A, B) A ## B
#define JoinExpand(A, B) JoinNoExpand(A, B)

#define LogInfo Logger(std::cout, std::source_location::current(), Logger::Color::LightGreen)
#define LogWarning Logger(std::cout, std::source_location::current(), Logger::Color::Yellow)
#define LogError Logger(std::cerr, std::source_location::current(), Logger::Color::Red)
#define LogColored(c) Logger(std::cout, std::source_location::current(), c)
#define LogDuration(message) DurationLogger JoinExpand(logger, __LINE__)(std::cout, std::source_location::current(), message)