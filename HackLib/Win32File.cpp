#include "Win32File.hpp"
#include "Exceptions.hpp"

Win32File::Win32File(const std::filesystem::path& path, DWORD access) :
	Win32Handle(CreateFileW(
		path.c_str(),
		access,
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

	LARGE_INTEGER size;
	Clear(size);

	if (!GetFileSizeEx(_handle, &size))
	{
		throw Win32Exception("GetFileSizeEx");
	}

	// I do not need support for over 4GiB for now

	if (size.HighPart)
	{
		throw OutOfRangeException("Too large file to handle");
	}

	_size = size.LowPart;
}

HANDLE Win32File::Value() const
{
	return _handle;
}

size_t Win32File::Size() const
{
	return _size;
}

bool Win32File::IsValid() const
{
	return Win32Handle::IsValid() && _ovl.Offset <= _size;
}

size_t Win32File::Read(void* buffer, size_t size)
{
	DWORD bytesRead = 0;
	
	if (!ReadFile(_handle, buffer, static_cast<DWORD>(size), &bytesRead, &_ovl))
	{
		throw Win32Exception("ReadFile");
	}

	_ovl.Offset += bytesRead;
	return bytesRead;
}

size_t Win32File::ReadAt(void* buffer, size_t size, size_t offset)
{
	DWORD bytesRead = 0;
	_ovl.Offset = static_cast<DWORD>(offset);

	if (!ReadFile(_handle, buffer, static_cast<DWORD>(size), &bytesRead, &_ovl))
	{
		throw Win32Exception("ReadFile");
	}

	_ovl.Offset += bytesRead;
	return bytesRead;
}

size_t Win32File::Write(const void* buffer, size_t size)
{
	DWORD bytesWritten = 0;

	if (!WriteFile(_handle, buffer, static_cast<DWORD>(size), &bytesWritten, &_ovl))
	{
		throw Win32Exception("WriteFile");
	}

	_ovl.Offset += bytesWritten;
	return bytesWritten;
}

size_t Win32File::WriteAt(size_t offset, const void* buffer, size_t size)
{
	DWORD bytesWritten = 0;
	_ovl.Offset = static_cast<DWORD>(offset);

	if (!WriteFile(_handle, buffer, static_cast<DWORD>(size), &bytesWritten, &_ovl))
	{
		throw Win32Exception("WriteFile");
	}

	_ovl.Offset += bytesWritten;
	return bytesWritten;
}

std::string Win32File::ReadUntil(char byte)
{
	std::string result;

	std::string buffer(0xFF, '\0');

	while (Read(buffer.data(), buffer.size()))
	{
		size_t terminator = buffer.find(byte);

		if (terminator == std::string::npos)
		{
			result.append(buffer);
			continue;
		}
		
		result.append(buffer, 0, terminator);
		_ovl.Offset -= static_cast<DWORD>(buffer.size());
		_ovl.Offset += static_cast<DWORD>(terminator + 1);
		break;
	}

	return result;
}

std::string Win32File::ReadAtUntil(size_t offset, char byte)
{
	SetPosition(offset);
	return ReadUntil(byte);
}

size_t Win32File::CurrentPosition() const
{
	return _ovl.Offset;
}

void Win32File::SetPosition(size_t position)
{
	_ovl.Offset = static_cast<DWORD>(position);
}