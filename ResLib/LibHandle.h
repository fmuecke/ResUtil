#pragma once
#include <windows.h>

struct LibHandle
{
	LibHandle(const char* fileName) : handle{ ::LoadLibraryA(fileName) } {}
	~LibHandle() { if (IsValid()) ::FreeLibrary(handle); }
	bool IsValid() const { return handle != 0; }

	HMODULE handle;
};
