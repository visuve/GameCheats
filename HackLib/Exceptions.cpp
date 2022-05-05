#include "Exceptions.hpp"

#include <winternl.h>

const char* WindowsErrorCategory::name() const noexcept
{
	return "Windows Error Category";
}

std::string WindowsErrorCategory::message(DWORD error) const
{
	_ASSERT(error != ERROR_SUCCESS);

	HMODULE module = nullptr;
	std::array<char, 0x400> buffer = {};
	DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

	if (error > 0xFFFF)
	{
		module = GetModuleHandleW(L"ntdll.dll");

		if (!module)
		{
			// We are totally screwed at this point
			return "unknown error";
		}

		const FARPROC ntStatusToDosErrorAddress =
			GetProcAddress(module, "RtlNtStatusToDosError");

		const auto ntStatusToDosErrorFunction =
			reinterpret_cast<decltype(&RtlNtStatusToDosError)>(ntStatusToDosErrorAddress);

		if (ntStatusToDosErrorFunction)
		{
			ULONG dosError = ntStatusToDosErrorFunction(error);

			if (dosError != ERROR_SUCCESS && dosError != ERROR_MR_MID_NOT_FOUND)
			{
				error = static_cast<DWORD>(dosError);
			}
		}

		flags |= FORMAT_MESSAGE_FROM_HMODULE;
	}

	DWORD size = FormatMessageA(
		flags,
		module,
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