#include "stdafx.h"
#include "Handle.hpp"
#include "CmdArgs.hpp"
#include "CmdArgsParser.hpp"
#include "ResLib/ResLib.hpp"
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
#include "main.h"

using namespace std;

int main(int argc, char** argv)
{
	CmdArgsParser argsParser{ "ResUtil v0.3 (c) 2015 Florian Muecke" };

	argsParser.Add({ "write", "write raw data into the specified file resource",
	{
		{"in", "the file containing the raw data" },
		{"out", "the target file" },
		{"type", "the type of the resouce (see below)" },
		{"id", "the resource id" },
		//{"lang", "the language id", CmdArgsParser::RequiredArg::no }
		} });

	argsParser.Add({ "read", "read the specified resource and dump it to disk",
	{
		{ "in", "the source file" },
		{ "out", "the target file" },
		{ "type", "the type of the resouce (see below)" },
		{ "id", "the resource id" },
		//{ "lang", "the language id" }
	} });

    argsParser.Add({ "enum", "enumerate resources of a given type",
    {
        { "in", "the source file" },
        { "type", "the type of the resouces (see below)" },
        //{ "lang", "the language id" }
    } });

    argsParser.Add({ "copy", "copy a resource from one file to another",
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
		argsParser.AddAdditionalHelp(str.substr(0, str.size() - 2));
	}
	try
	{
		argsParser.Parse(argc, argv);
	}
	catch (CmdArgsParser::InvalidCommandArgsException& e)
	{
		cerr << argsParser.HelpText(e.Command());
		cerr << "\n" << e.what() << "\n";
		return ERROR_BAD_ARGUMENTS;
	}
	catch (CmdArgsParser::ParseException& e)
	{
		cerr << argsParser.HelpText();
		cerr << "\n" << e.what() << "\n";
		return ERROR_BAD_ARGUMENTS;
	}

	try
	{
		if (ResLib::Types.find(argsParser.GetValue("type")) == cend(ResLib::Types))
		{
			cerr << argsParser.HelpText();
			cerr << "\nerror: invalid resource type: " << argsParser.GetValue("type") << ". Valid types are:\n";
			for (auto const& r : ResLib::Types) cout << "\t" << r.first << "\n";
			return ERROR_BAD_ARGUMENTS;
		}

		if (argsParser.GetCommand() == "write")
		{
			//auto lang = langId.empty() ? WORD(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)) : static_cast<WORD>(stoi(langId));
			auto data = ResUtil::ReadData(argsParser.GetValue("in").c_str());
			ResLib::Write(data, argsParser.GetValue("out").c_str(), argsParser.GetValue("type").c_str(), stoi(argsParser.GetValue("id")));
		}
		else if (argsParser.GetCommand() == "read")
		{
			auto data = ResLib::Read(
				argsParser.GetValue("in").c_str(),
				argsParser.GetValue("type").c_str(),
				stoi(argsParser.GetValue("id")));

			ResUtil::WriteData(data, argsParser.GetValue("out").c_str());
		}
		else if (argsParser.GetCommand() == "clone")
		{
			throw std::exception("NOT IMPLEMENTED");
		}
        else if (argsParser.GetCommand() == "enum")
        {
            auto data = ResLib::Enum(
                argsParser.GetValue("in").c_str(),
                argsParser.GetValue("type").c_str());
            
            for (auto i : data)
            {
                cout << i << "\n";
            }
        }
		else
		{
			cerr << argsParser.HelpText();
			return ERROR_BAD_ARGUMENTS;
		}
	}
	catch (std::exception& e)
	{
		cerr << "\nerror: " << e.what();
		return 1;
	}
}

