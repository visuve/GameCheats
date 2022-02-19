#pragma once

#include "NonCopyable.hpp"

class AutoHandle
{
public:
	AutoHandle() = default;
	AutoHandle(void* handle);
	NonCopyable(AutoHandle);
	virtual ~AutoHandle();

	operator void*() const;

private:
	void Reset(void* handle = nullptr);
	void* _handle = nullptr;
};