#pragma once
#include <Windows.h>

struct Handle
{
	Handle(HANDLE handle) noexcept: _handle{ handle } {}
	~Handle()
	{
		if (IsValid()) ::CloseHandle(_handle);
	}

	operator HANDLE() const { return _handle; }
	bool IsValid() const { return _handle != INVALID_HANDLE_VALUE && _handle != NULL; }

private:
	HANDLE _handle;
};
