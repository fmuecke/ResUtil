#include "stdafx.h"
#include "Handle.hpp"
#include "CmdArgs.hpp"
#include "CmdArgsParser.hpp"
#include "ResLib\ResLib.h"
#include "ResUtil.h"
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

int main(int argc, char** argv)
{
	CmdArgsParser p( "ResUtil v0.2 (c) 2015 Florian Muecke" );

	p.Add({ "write", "write raw data into the specified file resource",
	{
		{"in", "the file containing the raw data" },
		{"out", "the target file" },
		{"type", "the type of the resouce (see below)" },
		{"id", "the resource id" },
		//{"lang", "the language id", CmdArgsParser::RequiredArg::no }
		} });

	p.Add({ "read", "read the specified resource and dump it to disk",
	{
		{ "in", "the source file" },
		{ "out", "the target file" },
		{ "type", "the type of the resouce (see below)" },
		{ "id", "the resource id" },
		//{ "lang", "the language id" }
	} });

	p.Add({ "copy", "copy a resource from one file to another",
	{
		{ "in", "the source file" },
		{ "out", "the target file" },
		{ "type", "the type of the resouce (see below)" },
		{ "idIn", "the resource id in the source file" },
		{ "idOut", "the resource id for the target file" },
		//{ "langIn", "the language id of the resource in the source file" },
		//{ "langOut", "the language id", CmdArgsParser::RequiredArg::no }
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

	cout << p.HelpText();
	try
	{
		ResUtil rr;
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
			rr.PrintUsage();
			return ERROR_BAD_ARGUMENTS;
		}

		if (ResLib::Types.find(resType) == cend(ResLib::Types))
		{
			rr.PrintUsage();
			cerr << "\nInvalid resource type: " << resType << ". Valid types are:\n";
			for (auto const& r : ResLib::Types) cout << "\t" << r.first << "\n";
			return ERROR_BAD_ARGUMENTS;
		}

		if (!rawData.empty())
		{
			//auto lang = langId.empty() ? WORD(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)) : static_cast<WORD>(stoi(langId));
			auto data = rr.ReadData(rawData.c_str());
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
				rr.PrintUsage();
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

