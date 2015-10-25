#include "stdafx.h"
#include "Handle.hpp"
#include "CmdArgs.hpp"
#include "CmdArgsParser.hpp"
#include "ResLib\ResLib.h"
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

static void Usage()
{
	cout
		<< "ReplaceRes v0.1 (c) 2015 Florian Muecke\n\n"
		<< "usage: (1) ReplaceRes.exe write /target /id /type (/langId) /rawData\n"
		<< "       (2) ReplaceRes.exe copy /target:file.ext /id:resID /type:resType (/langId:languageID) /source:file.ext /sourceId:resID (/sourceLangId:languageID)\n";
}

static error_code GetError()
{
	return error_code(::GetLastError(), system_category());
}

struct IoException : public std::exception
{
	IoException(const char* msg) :_msg{ msg } {}

	virtual const char* what() const override { return _msg.c_str(); }

private:
	string _msg;
};

static vector<char> ReadData(const char* fileName)
{
	Handle file = { ::CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) };
	if (!file.IsValid())
	{
		auto msg = string("Unable to open target file: ") + GetError().message();
		throw IoException(msg.c_str());
	}

	DWORD size = ::GetFileSize(file, nullptr);
	vector<char> data(size, 0x00);
	if (0 == ReadFile(file, data.data(), static_cast<DWORD>(data.size()), &size, nullptr))
	{
		auto err = GetError();
		auto msg = string("Unable to read data: ") + err.message();
		throw IoException(msg.c_str());
	}

	return data;
}


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

	p.Add({ "read", "read the specified resource and dump it to disk",
	{
		{ "source", "the source file" },
		{ "id", "the resource id" },
		{ "langId", "the language id" },
		{ "resType", "the type of the resouce (see below)" },
		{ "target", "the target file" },
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

	{
		stringstream resTypesHelp;
		resTypesHelp << "Valid resource types are: ";
		for (auto const& x : ResLib::Types)
		{
			resTypesHelp << x.first << ", ";
		}
		auto str = resTypesHelp.str();
		p.AddAdditionalHelp(str.substr(0, str.size() - 2));
	}

	//cout << p.HelpText();
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

		if (ResLib::Types.find(resType) == cend(ResLib::Types))
		{
			Usage();
			cerr << "\nInvalid resource type: " << resType << ". Valid types are:\n";
			for (auto const& r : ResLib::Types) cout << "\t" << r.first << "\n";
			return ERROR_BAD_ARGUMENTS;
		}

		if (!rawData.empty())
		{
			//auto lang = langId.empty() ? WORD(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)) : static_cast<WORD>(stoi(langId));
			auto data = ReadData(rawData.c_str());

			ResLib::Write(data, targetFile.c_str(), resType.c_str(), stoi(resId));


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

