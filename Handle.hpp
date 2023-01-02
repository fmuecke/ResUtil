#pragma once
#include <Windows.h>

struct Handle
{
	Handle(HANDLE handle) noexcept : _handle{ handle } {}
	~Handle()
	{
		if (IsValid()) ::CloseHandle(_handle);
	}
	
	Handle() = delete;
	Handle(const Handle&) = delete;
	Handle(Handle&&) = delete;
	Handle& operator=(const Handle&) = delete;
	Handle& operator=(Handle&&) = delete;

	operator HANDLE() const noexcept { return _handle; }
	bool IsValid() const noexcept { return _handle != INVALID_HANDLE_VALUE && !IsNull(_handle); }
	static bool IsNull(HANDLE h) noexcept { return h == nullptr; }

private:
	HANDLE _handle;
};

