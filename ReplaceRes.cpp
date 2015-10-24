#include "stdafx.h"
#include "Handle.hpp"
#include "CmdArgs.hpp"
#include "CmdArgsParser.hpp"
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
		<< "usage: (1) ReplaceRes.exe write /target /id /type (/langId) /rawData\n"
		<< "       (2) ReplaceRes.exe copy /target:file.ext /id:resID /type:resType (/langId:languageID) /source:file.ext /sourceId:resID (/sourceLangId:languageID)\n";
}



error_code GetError() 
{
	return error_code(::GetLastError(), system_category());
}

vector<char> ReadData(const char* fileName)
{
	Handle file = { ::CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) };
	if (!file.IsValid())
	{
		auto err = GetError();
		auto msg = string("Unable to open target file: ") + err.message();
		throw std::exception(msg.c_str(), err.value());
	}

	DWORD size = ::GetFileSize(file, nullptr);
	vector<char> data(size, 0x00);
	if (0 == ReadFile(file, data.data(), static_cast<DWORD>(data.size()), &size, nullptr))
	{
		auto err = GetError();
		auto msg = string("Unable to read data: ") + err.message();
		throw std::exception(msg.c_str(), err.value());
	}

	return data;
}

map<string, char*> ResTypes
{
	{ "accelerator", MAKEINTRESOURCEA(9) }, // Accelerator table.
	{ "anicursor", MAKEINTRESOURCEA(21) }, // Animated cursor.
	{ "aniicon", MAKEINTRESOURCEA(22) }, // Animated icon.
	{ "bitmap", MAKEINTRESOURCEA(2) }, // Bitmap resource.
	{ "cursor", MAKEINTRESOURCEA(1) }, // Hardware - dependent cursor resource.
	{ "dialog", MAKEINTRESOURCEA(5) }, // Dialog box.
	{ "dlginclude", MAKEINTRESOURCEA(17) }, // Allows a resource editing tool to associate a string with an.rc file.Typically, the string is the name of the header file that provides symbolic names.The resource compiler parses the string but otherwise ignores the value.For example, 1 DLGINCLUDE "MyFile.h"
	{ "font", MAKEINTRESOURCEA(8) }, // Font resource.
	{ "fontdir", MAKEINTRESOURCEA(7) }, // Font directory resource.
	{ "groupcursor", MAKEINTRESOURCEA((ULONG_PTR)(RT_CURSOR)+11) }, // Hardware - independent cursor resource.
	{ "groupicon", MAKEINTRESOURCEA((ULONG_PTR)(RT_ICON)+11) }, // Hardware - independent icon resource.
	{ "html", MAKEINTRESOURCEA(23) }, // HTML resource.
	{ "icon", MAKEINTRESOURCEA(3) }, // Hardware - dependent icon resource.
	{ "manifest", MAKEINTRESOURCEA(24) }, // Side - by - Side Assembly Manifest.
	{ "menu", MAKEINTRESOURCEA(4) }, // Menu resource.
	{ "messagetable", MAKEINTRESOURCEA(11) }, // Message - table entry.
	{ "plugplay", MAKEINTRESOURCEA(19) }, // Plug and Play resource.
	{ "rcdata", MAKEINTRESOURCEA(10) }, // Application - defined resource(raw data).
	{ "string", MAKEINTRESOURCEA(6) }, // String - table entry.
	{ "version", MAKEINTRESOURCEA(16) }, // Version resource.
	{ "vxd", MAKEINTRESOURCEA(20) } // VXD.
};

int main(int argc, char** argv)
{
	CmdArgsParser p;
	p.Add({ "write", "write raw data into the specified file resource",
	{
		{"target", "the target file" },
		{"id", "the resource id" },
		{"langId", "the language id", CmdArgsParser::RequiredArg::no },
		{"resType", "the type of the resouce (see below)" },
		{"rawData", "the file containing the raw data" }
		} });

	p.Add({ "copy", "copy a resource from one file to another",
	{
		{ "target", "the target file" },
		{ "id", "the resource id for the target file" },
		{ "langId", "the language id", CmdArgsParser::RequiredArg::no },
		{ "resType", "the type of the resouce (see below)" },
		{ "source", "the source file" },
		{ "sourceId", "the resource id in the source file" },
		{ "sourceLangId", "the language id of the resource in the source file" }
	} });

	p.Add({ "dump", "dump the specified resource to disk",
	{
		{ "source", "the source file" },
		{ "id", "the resource id" },
		{ "langId", "the language id" },
		{ "resType", "the type of the resouce (see below)" },
		{ "target", "the target file" },
	} });

	{
		stringstream resTypesHelp;
		resTypesHelp << "Valid resource types are: ";
		for (auto const& x : ResTypes)
		{
			resTypesHelp << x.first << ", ";
		}
		auto str = resTypesHelp.str();
		p.AddAdditionalHelp(str.substr(0, str.size() - 2));
	}

	cout << p.HelpText();
	try
	{
		auto args = CmdArgs<char>(argc, argv);
		auto targetFile = args.TakeArg("/target:");
		auto resId = args.TakeArg("/id:");
		//auto langId = args.TakeArg("/langId:");
		auto rawData = args.TakeArg("/rawData:");
		auto resType = args.TakeArg("/type:");
		auto sourceFile = args.TakeArg("/source:");
		auto sourceResId = args.TakeArg("/sourceId:");
		auto sourceLangId = args.TakeArg("/sourceLangId:");

		if (targetFile.empty() || resId.empty() || resId.empty() || resType.empty())
		{
			Usage();
			return ERROR_BAD_ARGUMENTS;
		}

		if (ResTypes.find(resType) == cend(ResTypes))
		{
			Usage();
			cerr << "\nInvalid resource type: " << resType << ". Valid types are:\n";
			for (auto const& r : ResTypes) cout << "\t" << r.first << "\n";
			return ERROR_BAD_ARGUMENTS;
		}

		if (!rawData.empty())
		{
			auto targetFileHandle = ::BeginUpdateResourceA(targetFile.c_str(), FALSE);
			if (targetFileHandle == NULL)
			{
				auto err = GetError();
				cerr << "Opening file '" << targetFile << L"' failed: ";
				cout << err.message() << endl;
				return err.value();
			}
			auto id = stoi(resId);
			//auto lang = langId.empty() ? WORD(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)) : static_cast<WORD>(stoi(langId));
			auto data = ReadData(rawData.c_str());

			while (data.size() % 4) data.push_back(0x00);  // data must be aligned

			if (::UpdateResourceA(
				targetFileHandle, 
				ResTypes[resType], 
				MAKEINTRESOURCEA(id), 
				MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), 
				data.data(), 
				static_cast<DWORD>(data.size())) == 0)
			{
				auto err = GetError();
				cerr << "Updating resource failed: " << err.message() << endl;
				return err.value();
			}

			if (::EndUpdateResourceA(targetFileHandle, false) == 0)
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

