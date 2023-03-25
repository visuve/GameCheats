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

Win32FileMapping::Win32FileMapping(const Win32File& file) :
	Win32Handle(CreateFileMappingW(file.Value(), nullptr, PAGE_READONLY, 0, 0, nullptr))
{
	if (!_handle)
	{
		throw Win32Exception("CreateFileMappingW");
	}
}

Win32FileMapping::~Win32FileMapping()
{
}

Win32FileView::Win32FileView(const Win32FileMapping& mapping) :
	_view(MapViewOfFile(mapping.Value(), FILE_MAP_READ, 0, 0, 0))
{
	if (!_view)
	{
		throw Win32Exception("MapViewOfFile");
	}
}

Win32FileView::~Win32FileView()
{
	if (_view)
	{
		UnmapViewOfFile(_view);
	}
}
