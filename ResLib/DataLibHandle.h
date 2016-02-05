#pragma once
#include <windows.h>
#include "..\Utf8.hpp"

struct DataLibHandle
{
	DataLibHandle(const char* fileName)
        : handle{ 
            ::LoadLibraryExW(
                Utf8::ToWide(fileName).c_str(), 
                0, 
                LOAD_LIBRARY_AS_DATAFILE|LOAD_LIBRARY_AS_IMAGE_RESOURCE) } 
    {}

	~DataLibHandle() { if (IsValid()) ::FreeLibrary(handle); }
	
    bool IsValid() const { return handle != 0; }

	HMODULE handle;
};
