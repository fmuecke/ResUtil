#pragma once
#include <windows.h>

struct DataLibHandle
{
	DataLibHandle(const char* fileName) : handle{ ::LoadLibraryExA(fileName, 0, LOAD_LIBRARY_AS_DATAFILE|LOAD_LIBRARY_AS_IMAGE_RESOURCE) } {}
	~DataLibHandle() { if (IsValid()) ::FreeLibrary(handle); }
	bool IsValid() const { return handle != 0; }

	HMODULE handle;
};
