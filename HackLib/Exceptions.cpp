#include "Exceptions.hpp"

#include <winternl.h>

const char* WindowsErrorCategory::name() const noexcept
{
	return "Windows Error Category";
}

std::string WindowsErrorCategory::message(DWORD error) const
{
	const HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");

	if (!ntdll)
	{
		// We are totally screwed at this point
		return "unknown error";
	}

	const FARPROC ntStatusToDosErrorAddress =
		GetProcAddress(ntdll, "RtlNtStatusToDosError");

	auto ntStatusToDosErrorFunction =
		reinterpret_cast<decltype(&RtlNtStatusToDosError)>(ntStatusToDosErrorAddress);

	if (ntStatusToDosErrorFunction)
	{
		// This is not a must, do not return here
		error = ntStatusToDosErrorFunction(error);
	}

	constexpr DWORD flags =
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_FROM_HMODULE |
		FORMAT_MESSAGE_IGNORE_INSERTS;

	std::array<char, 0x400> buffer = {};

	DWORD size = FormatMessageA(
		flags,
		ntdll,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buffer.data(),
		static_cast<DWORD>(buffer.size()),
		nullptr);

	if (size <= 2)
	{
		return "unknown error";
	}

	// Do not include the CRLF FormatMessageA/W adds
	return { buffer.data(), size - 2 };
}

std::string WindowsErrorCategory::message(int error) const
{
	return message(static_cast<DWORD>(error));
}

std::string ExceptionMessage(std::string_view message, const std::source_location& location)
{
	return std::format("{0}:{1}: {2}!",
		std::filesystem::path(location.file_name()).filename().string(),
		location.line(),
		message);
}