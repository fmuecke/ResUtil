#include "stdafx.h"
#include "Handle.hpp"
#include "CmdArgs.hpp"
#include "CmdArgsParser.hpp"
#include "ResLib/ResLib.hpp"
#include "ResUtil.h"
#include "StringHelper.h"

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


static const char* const strCommand_write = "write";
static const char* const strCommand_read = "read";
static const char* const strCommand_copy = "copy";
static const char* const strCommand_enum = "enum";
static const char* const strCommand_enumTypes = "enumTypes";

static const char* const strParam_in = "in";
static const char* const strParam_out = "out";
static const char* const strParam_type = "type";
static const char* const strParam_id = "id";
static const char* const strParam_idIn = "idIn";
static const char* const strParam_idOut = "idOut";

int wmain(int argc, wchar_t** argv)
{
    CmdArgsParser argsParser{ "ResUtil v0.4 (c) 2015 Florian Muecke" };

    argsParser.Add({ strCommand_write, "write raw data into the specified file resource",
    {
        { strParam_in, "file containing the raw data" },
        { strParam_out, "target file" },
        { strParam_type, "type of the resouce (see below)" },
        { strParam_id, "resource id" },
        //{"lang", "language id", CmdArgsParser::RequiredArg::no }
        } });

    argsParser.Add({ strCommand_read, "read the specified resource and dump it to disk",
    {
        { strParam_in, "source file" },
        { strParam_out, "target file" },
        { strParam_type, "type of the resouce (see below)" },
        { strParam_id, "resource id" },
        //{ "lang", "the language id" }
    } });

    argsParser.Add({ strCommand_enum, "enumerate resources of a given type",
    {
        { strParam_in, "source file" },
        { strParam_type, "type of the resouces (see below)" },
        //{ "lang", "language id" }
    } });

    argsParser.Add({ strCommand_enumTypes, "enumerate resources types",
    {
        { strParam_in, "source file" },
        //{ "lang", "language id" }
    } });

    //argsParser.Add({ strCommand_copy, "copy a resource from one file to another",
    //{
    //    { strParam_in, "source file" },
    //    { strParam_out, "target file" },
    //    { strParam_type, "type of the resouce (see below)" },
    //    { strParam_idIn, "resource id in the source file" },
    //    { strParam_idOut, "resource id for the target file" },
    //    //{ "langIn", "language id of the resource in the source file" },
    //    //{ "langOut", "language id", CmdArgsParser::RequiredArg::no }
    //} });

    {
        stringstream helpText;
        helpText << "Predefined resource types are: ";
        helpText << StringHelper::join(ResTypes::ResNameToValueMap) << endl;
        helpText << "Custom types can be specified as strings";
        argsParser.AddAdditionalHelp(helpText.str());
    }
    try
    {
        argsParser.Parse(argc, argv);
    }
    catch (const CmdArgsParser::InvalidCommandArgsException& e)
    {
        cerr << argsParser.HelpText(e.Command());
        cerr << "\n" << e.what() << "\n";
        return ERROR_BAD_ARGUMENTS;
    }
    catch (CmdArgsParser::ParseException const& e)
    {
        cerr << argsParser.HelpText();
        cerr << "\n" << e.what() << "\n";
        return ERROR_BAD_ARGUMENTS;
    }

    try
    {
        if (argsParser.GetCommand() == strCommand_enumTypes)
        {
            auto types = ResLib::EnumerateTypes(argsParser.GetValue(strParam_in).c_str());
            cout << StringHelper::join(types, "\n") << endl;
        }
        else
        {
            if (argsParser.GetCommand() == strCommand_write)
            {
                //auto lang = langId.empty() ? WORD(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)) : static_cast<WORD>(stoi(langId));
                auto data = ResUtil::ReadData(argsParser.GetValue(strParam_in).c_str());
                ResLib::Write(data, argsParser.GetValue(strParam_out).c_str(), argsParser.GetValue(strParam_type).c_str(), argsParser.GetValue(strParam_id).c_str());
            }
            else if (argsParser.GetCommand() == strCommand_read)
            {
                auto data = ResLib::Read(
                    argsParser.GetValue(strParam_in).c_str(),
                    argsParser.GetValue(strParam_type).c_str(),
                    argsParser.GetValue(strParam_id).c_str());

                ResUtil::WriteData(data, argsParser.GetValue(strParam_out).c_str());
            }
            else if (argsParser.GetCommand() == strCommand_enum)
            {
                auto data = ResLib::Enum(
                    argsParser.GetValue(strParam_in).c_str(),
                    argsParser.GetValue(strParam_type).c_str());

                cout << StringHelper::join(data, "\n") << endl;
            }
            //else if (argsParser.GetCommand() == strCommand_copy)
            //{
            //    throw std::exception("NOT IMPLEMENTED");
            //}
            else
            {
                cerr << argsParser.HelpText();
                return ERROR_BAD_ARGUMENTS;
            }
        }
    }
    catch (const std::exception& e)
    {
        cerr << "\nerror: " << e.what();
        return 1;
    }
}

