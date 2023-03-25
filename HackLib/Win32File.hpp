#pragma once

#include "NonCopyable.hpp"
#include "Win32Handle.hpp"

class Win32File : public Win32Handle
{
public:
	explicit Win32File(const std::filesystem::path& path);
	virtual ~Win32File();

	NonCopyable(Win32File);

	inline HANDLE Value() const
	{
		return _handle;
	}

	size_t Size() const;
	size_t Read(void* buffer, size_t size) const;
};

class Win32FileMapping : public Win32Handle
{
public:
	Win32FileMapping(const Win32File& file);
	virtual ~Win32FileMapping();

	NonCopyable(Win32FileMapping);

	inline HANDLE Value() const
	{
		return _handle;
	}
};

class Win32FileView
{
public:
	explicit Win32FileView(const Win32FileMapping& mapping);
	~Win32FileView();

	NonCopyable(Win32FileView);

	inline void* Data() const
	{
		return _view;
	}

private:
	void* _view = nullptr;
};