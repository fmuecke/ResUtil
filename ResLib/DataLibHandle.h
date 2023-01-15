#pragma once

#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "..\Utf8.hpp"

struct DataLibHandle
{
	DataLibHandle(const char* fileName)
        : handle{ 
            ::LoadLibraryExW(
                Utf8::ToWide(fileName).c_str(), 
                nullptr, 
                LOAD_LIBRARY_AS_DATAFILE|LOAD_LIBRARY_AS_IMAGE_RESOURCE) } 
    {}

	~DataLibHandle() { if (IsValid()) ::FreeLibrary(handle); }

	DataLibHandle() = delete;
	DataLibHandle(DataLibHandle&) = delete;
	DataLibHandle(const DataLibHandle&) = delete;
	DataLibHandle(DataLibHandle&&) = delete;
	DataLibHandle& operator=(const DataLibHandle&) = delete;
	DataLibHandle& operator=(DataLibHandle&&) = delete;
	
	operator HMODULE() const noexcept { return handle; }
    bool IsValid() const noexcept { return handle != nullptr; }

private:
	HMODULE handle;
};
