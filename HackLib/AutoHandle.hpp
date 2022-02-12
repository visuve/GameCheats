#pragma once

#include "NonCopyable.hpp"

class AutoHandle
{
public:
	AutoHandle() = default;
	AutoHandle(HANDLE handle);
	NonCopyable(AutoHandle);
	virtual ~AutoHandle();

	operator HANDLE() const;

private:
	void Reset(HANDLE handle = nullptr);
	HANDLE _handle = nullptr;
};