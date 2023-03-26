#include "Win32File.hpp"
#include "Exceptions.hpp"

Win32File::Win32File(const std::filesystem::path& path) :
	Win32Handle(CreateFileW(
		path.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr))
{
	if (!IsValid())
	{
		throw Win32Exception("CreateFileW");
	}
}

Win32File::~Win32File()
{
}

size_t Win32File::Size() const
{
	LARGE_INTEGER size = { 0 };

	if (!GetFileSizeEx(_handle, &size))
	{
		throw Win32Exception("GetFileSizeEx");
	}

	return size.QuadPart;
}

size_t Win32File::Read(void* buffer, size_t size) const
{
	DWORD bytesRead = 0;
	
	if (!ReadFile(_handle, buffer, static_cast<DWORD>(size), &bytesRead, nullptr))
	{
		throw Win32Exception("ReadFile");
	}

	return bytesRead;
}

size_t Win32File::ReadAt(void* buffer, size_t size, size_t offset) const
{
	DWORD bytesRead = 0;

	OVERLAPPED o = {};
	o.Pointer = reinterpret_cast<void*>(offset);

	if (!ReadFile(_handle, buffer, static_cast<DWORD>(size), &bytesRead, &o))
	{
		throw Win32Exception("ReadFile");
	}

	return bytesRead;
}

size_t Win32File::CurrentPosition() const
{
	LARGE_INTEGER distanceToMove = {};
	LARGE_INTEGER position = {};

	if (!SetFilePointerEx(_handle, distanceToMove, &position, FILE_CURRENT))
	{
		throw Win32Exception("SetFilePointerEx");
	}

	return position.QuadPart;
}
