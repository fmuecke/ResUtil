#pragma once
#include <windows.h>

struct DataLibHandle
{
	DataLibHandle(const char* fileName) : handle{ ::LoadLibraryExA(fileName, 0, LOAD_LIBRARY_AS_DATAFILE|DONT_RESOLVE_DLL_REFERENCES) } {}
	~DataLibHandle() { if (IsValid()) ::FreeLibrary(handle); }
	bool IsValid() const { return handle != 0; }

	HMODULE handle;
};
