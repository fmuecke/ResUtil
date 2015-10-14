#include "stdafx.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <system_error>
#include <map>
#include <Windows.h>

using namespace std;

void Usage()
{
	cout
		<< "ReplaceRes v0.1 (c) 2015 Florian Muecke\n\n"
		<< "usage: (1) ReplaceRes.exe /target:file.ext /id:resID /type:resType (/langId:languageID) /rawData:file.ext\n"
		<< "       (2) ReplaceRes.exe /target:file.ext /id:resID /type:resType (/langId:languageID) /source:file.ext /sourceId:resID (/sourceLangId:languageID)\n";
}

struct Handle 
{
	Handle(HANDLE handle) : _handle{ handle } {}
	~Handle() 
	{
		if (bool()) ::CloseHandle(_handle);
	}

	operator HANDLE() const { return _handle; }
	bool valid() const { return _handle != INVALID_HANDLE_VALUE; }

private:
	HANDLE _handle;
};

error_code GetError() 
{
	return error_code(::GetLastError(), system_category());
}

wstring TakeParam(vector<wstring>& args, wstring const& tag)
{
	auto pos = find_if(cbegin(args), cend(args), [&](wstring const& a)
	{
		return equal(cbegin(tag), cend(tag), cbegin(a));
	});

	if (pos == cend(args)) return L"";
	auto param = (*pos).substr(tag.size());
	args.erase(pos);
	
	return param;
}

vector<char> ReadData(const wchar_t* fileName)
{
	Handle file = { ::CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) };
	if (!file.valid())
	{
		auto err = GetError();
		auto msg = string("Unable to open target file: ") + err.message();
		throw std::exception(msg.c_str(), err.value());
	}

	DWORD size = ::GetFileSize(file, nullptr);
	vector<char> data(size, 0x00);
	if (0 == ReadFile(file, data.data(), data.size(), &size, nullptr))
	{
		auto err = GetError();
		auto msg = string("Unable to read data: ") + err.message();
		throw std::exception(msg.c_str(), err.value());
	}

	return data;
}

map<wstring, wchar_t*> ResTypes
{
	{ L"accelerator", MAKEINTRESOURCE(9) }, // Accelerator table.
	{ L"anicursor", MAKEINTRESOURCE(21) }, // Animated cursor.
	{ L"aniicon", MAKEINTRESOURCE(22) }, // Animated icon.
	{ L"bitmap", MAKEINTRESOURCE(2) }, // Bitmap resource.
	{ L"cursor", MAKEINTRESOURCE(1) }, // Hardware - dependent cursor resource.
	{ L"dialog", MAKEINTRESOURCE(5) }, // Dialog box.
	{ L"dlginclude", MAKEINTRESOURCE(17) }, // Allows a resource editing tool to associate a string with an.rc file.Typically, the string is the name of the header file that provides symbolic names.The resource compiler parses the string but otherwise ignores the value.For example, 1 DLGINCLUDE "MyFile.h"
	{ L"font", MAKEINTRESOURCE(8) }, // Font resource.
	{ L"fontdir", MAKEINTRESOURCE(7) }, // Font directory resource.
	{ L"groupcursor", MAKEINTRESOURCE((ULONG_PTR)(RT_CURSOR)+11) }, // Hardware - independent cursor resource.
	{ L"groupicon", MAKEINTRESOURCE((ULONG_PTR)(RT_ICON)+11) }, // Hardware - independent icon resource.
	{ L"html", MAKEINTRESOURCE(23) }, // HTML resource.
	{ L"icon", MAKEINTRESOURCE(3) }, // Hardware - dependent icon resource.
	{ L"manifest", MAKEINTRESOURCE(24) }, // Side - by - Side Assembly Manifest.
	{ L"menu", MAKEINTRESOURCE(4) }, // Menu resource.
	{ L"messagetable", MAKEINTRESOURCE(11) }, // Message - table entry.
	{ L"plugplay", MAKEINTRESOURCE(19) }, // Plug and Play resource.
	{ L"rcdata", MAKEINTRESOURCE(10) }, // Application - defined resource(raw data).
	{ L"string", MAKEINTRESOURCE(6) }, // String - table entry.
	{ L"version", MAKEINTRESOURCE(16) }, // Version resource.
	{ L"vxd", MAKEINTRESOURCE(20) } // VXD.
};

int wmain(int argc, wchar_t** argv)
{
	try
	{
		vector<wstring> args(argv, argv + argc);
		auto targetFile = TakeParam(args, L"/target:");
		auto resId = TakeParam(args, L"/id:");
		auto langId = TakeParam(args, L"/langId:");
		auto rawData = TakeParam(args, L"/rawData:");
		auto resType = TakeParam(args, L"/type:");
		auto sourceFile = TakeParam(args, L"/source:");
		auto sourceResId = TakeParam(args, L"/sourceId:");
		auto sourceLangId = TakeParam(args, L"/sourceLangId:");

		if (targetFile.empty() || resId.empty() || resId.empty() || resType.empty())
		{
			Usage();
			return ERROR_BAD_ARGUMENTS;
		}

		if (ResTypes.find(resType) == cend(ResTypes))
		{
			Usage();
			wcerr << L"\nInvalid resource type: " << resType << ". Valid types are:\n";
			for (auto const& r : ResTypes) wcout << "\t" << r.first << "\n";
			return ERROR_BAD_ARGUMENTS;
		}

		if (!rawData.empty())
		{
			auto targetFileHandle = ::BeginUpdateResourceW(targetFile.c_str(), FALSE);
			if (targetFileHandle == NULL)
			{
				auto err = GetError();
				wcerr << L"Opening file '" << targetFile << L"' failed: ";
				cout << err.message() << endl;
				return err.value();
			}
			auto id = stoi(resId);
			auto lang = langId.empty() ? MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL) : static_cast<WORD>(stoi(langId));
			auto data = ReadData(rawData.c_str());

			while (data.size() % 4) data.push_back(0x00);  // data must be aligned

			if (::UpdateResourceW(targetFileHandle, ResTypes[resType], MAKEINTRESOURCEW(id), lang, data.data(), data.size()) == 0)
			{
				auto err = GetError();
				cerr << "Updating resource failed: " << err.message() << endl;
				return err.value();
			}
			if (::EndUpdateResourceW(targetFileHandle, false) == 0)
			{
				auto err = GetError();
				cerr << "Resource updates could not be written: " << err.message() << endl;
				return err.value();
			}
		}
		else
		{
			if (!sourceFile.empty() && !sourceResId.empty())
			{
				cerr << "Not implemented yet";
				return 1;
			}
			else
			{
				Usage();
				return ERROR_BAD_ARGUMENTS;
			}
		}
	}
	catch (std::exception& e)
	{
		cerr << e.what();
		return 1;
	}
}

