#include "HackLib-PCH.hpp"
#include "AutoHandle.hpp"

AutoHandle::AutoHandle(HANDLE handle) :
	_handle(handle)
{
}

AutoHandle::~AutoHandle()
{
	Reset();
}

AutoHandle::operator HANDLE() const
{
	return _handle;
}

void AutoHandle::Reset(HANDLE handle)
{
	if (_handle == handle)
	{
		return;
	}

	if (_handle)
	{
		CloseHandle(_handle);
	}

	_handle = handle;
}
