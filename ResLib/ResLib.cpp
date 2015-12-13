#include "stdafx.h"
#include "ResLib.h"
#include "LibHandle.h"

#include <gsl.h>
#include <Windows.h>
#include <system_error>
#include <sstream>
#include <memory>
#include <iterator>
#include <algorithm>

using namespace std;
using Byte = unsigned char;

constexpr inline const char* IntResA(int i)
{
    [[suppress(type.1)]] return reinterpret_cast<const char*>(i);
}

constexpr inline WORD MakeLangId(int p, int s)
{
    return static_cast<WORD>(s) << 10 | static_cast<WORD>(p);
}

[[suppress(type.4), suppress(bounds.1)]]
const std::map<const std::string, const char*> ResLib::Types =
{
    { string("accelerator"), IntResA(9) }, // Accelerator table.
    { string("anicursor"), IntResA(21) }, // Animated cursor.
    { string("aniicon"), IntResA(22) }, // Animated icon.
    { string("bitmap"), IntResA(2) }, // Bitmap resource.
    { string("cursor"), IntResA(1) }, // Hardware - dependent cursor resource.
    { string("dialog"), IntResA(5) }, // Dialog box.
    { string("dlginclude"), IntResA(17) }, // Allows a resource editing tool to associate a string with an.rc file.Typically, the string is the name of the header file that provides symbolic names.The resource compiler parses the string but otherwise ignores the value.For example, 1 DLGINCLUDE "MyFile.h"
    { string("font"), IntResA(8) }, // Font resource.
    { string("fontdir"), IntResA(7) }, // Font directory resource.
    { string("groupcursor"), IntResA(12) }, // Hardware - independent cursor resource.
    { string("groupicon"), IntResA(14) }, // Hardware - independent icon resource.
    { string("html"), IntResA(23) }, // HTML resource.
    { string("icon"), IntResA(3) }, // Hardware - dependent icon resource.
    { string("manifest"), IntResA(24) }, // Side - by - Side Assembly Manifest.
    { string("menu"), IntResA(4) }, // Menu resource.
    { string("messagetable"), IntResA(11) }, // Message - table entry.
    { string("plugplay"), IntResA(19) }, // Plug and Play resource.
    { string("rcdata"), IntResA(10) }, // Application - defined resource(raw data).
    { string("string"), IntResA(6) }, // String - table entry.
    { string("version"), IntResA(16) }, // Version resource.
    { string("vxd"), IntResA(20) } // VXD.
};

static string GetError()
{
	auto code = ::GetLastError();
	auto err = error_code(code, system_category());
	auto msg = err.message();
	return msg.empty() ? "code = " + to_string(err.value()) + ")\n" : msg;
}

ResLib::ResLib()
{
}

ResLib::~ResLib()
{
}

void ResLib::Write(std::vector<unsigned char> const& data, const char* fileName, const char* resType, int resId/*, int langId*/)
{
	if (data.empty()) throw InvalidDataException();
	if (!fileName | !resType) throw ArgumentNullException();
	if (resId < 0 /*|| langId < 0*/) throw InvalidArgsException();
	auto resTypePos = Types.find(resType);
	if (resTypePos == Types.end()) throw InvalidTypeException(resType);

	auto targetFileHandle = ::BeginUpdateResourceA(fileName, FALSE);
	if (targetFileHandle == NULL)
	{
		auto err = GetError();
		stringstream msg;
		msg << "Opening file '" << fileName << "' failed: " << err << endl;
		throw InvalidFileException(msg.str().c_str());
	}

	auto _data = data;

	while (_data.size() % 8) _data.emplace_back(0x00);  // data must be aligned

	if (::UpdateResourceA(
		targetFileHandle, 
		resTypePos->second,
		IntResA(resId), 
        MakeLangId(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		_data.data(), 
		static_cast<DWORD>(data.size())) == 0)
	{
		auto err = GetError();
		stringstream msg; 
		msg << "Updating resource failed: " << err << endl;
		throw UpdateResourceException(msg.str().c_str());
	}
	if (::EndUpdateResourceA(targetFileHandle, false) == 0)
	{
		auto err = GetError();
		stringstream msg;
		msg << "Resource update could not be written: " << err << endl;
		throw UpdateResourceException(msg.str().c_str());
	}
}

std::vector<unsigned char> ResLib::Read(const char* fileName, const char* resType, int resId/*, int langId*/)
{
	if (!fileName | !resType) throw ArgumentNullException();
	auto resTypePos = Types.find(resType);
	if (resTypePos == Types.end()) throw InvalidTypeException(resType);
	if (resId < 0 /*|| langId < 0*/) throw InvalidArgsException();

	auto dll = LibHandle(fileName);
	if (!dll.IsValid())
	{
		stringstream msg;
		msg << "Unable to open file '" << fileName << endl;
		throw InvalidFileException(msg.str().c_str());
	}

	auto resInfo = ::FindResourceA(dll.handle, MAKEINTRESOURCEA(resId), resTypePos->second);
	if (!resInfo)
	{
		auto err = GetError();
		stringstream msg;
		msg << "Finding resouce with id=" << resId << " in file '" << fileName << "' failed: " << err << endl;
		throw InvalidResourceException(msg.str().c_str());
	}

	auto resSize = ::SizeofResource(dll.handle, resInfo);
	if (resSize == 0)
	{
		auto err = GetError();
		stringstream msg;
		msg << "Error getting size of resource with id=" << resId << " in file '" << fileName << "': " << err << endl;
		throw InvalidResourceException(msg.str().c_str());
	}

	auto resHandle = ::LoadResource(dll.handle, resInfo);
	if (!resHandle)
	{
		auto err = GetError();
		stringstream msg;
		msg << "Error loading resource id=" << resId << " in file '" << fileName << "': " << err << endl;
		throw InvalidResourceException(msg.str().c_str());
	}

    [[suppress(type.1)]]
	auto pResData = reinterpret_cast<Byte*>(::LockResource(resHandle));
	if (!pResData)
	{
		auto err = GetError();
		stringstream msg;
		msg << "Error locking resource id=" << resId << " in file '" << fileName << "': " << err << endl;
		throw InvalidResourceException(msg.str().c_str());
	}

    auto resData = gsl::span<Byte>(pResData, resSize);
    vector<Byte> data(resData.begin(), resData.end());

    return data;
}

void ResLib::Copy(const char* fromFile, const char* resType, int fromId, /*int fromLangId, */const char* toFile, int toId/*, int toLangId*/)
{
	auto const& data = Read(fromFile, resType, fromId);
	Write(data, toFile, resType, toId);
}

