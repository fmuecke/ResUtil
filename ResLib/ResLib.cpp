#include "stdafx.h"
#include "ResLib.h"
#include <Windows.h>
#include <system_error>
#include <sstream>
#include <memory>
#include <iterator>
#include <algorithm>
#include "LibHandle.h"

using namespace std;

static error_code GetError()
{
	return error_code(::GetLastError(), system_category());
}

const std::map<const std::string, const char*> ResLib::Types =
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
		msg << "Opening file '" << fileName << "' failed: "
		    << err.message() << endl;
		throw InvalidFileException(msg.str().c_str());
	}

	auto _data = data;

	while (_data.size() % 8) _data.emplace_back(0x00);  // data must be aligned

	if (::UpdateResourceA(
		targetFileHandle, 
		resTypePos->second,
		MAKEINTRESOURCEA(resId), 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), 
		_data.data(), 
		static_cast<DWORD>(data.size())) == 0)
	{
		stringstream msg; 
		msg << "Updating resource failed: " << GetError().message() << endl;
		throw UpdateResourceException(msg.str().c_str());
	}
	if (::EndUpdateResourceA(targetFileHandle, false) == 0)
	{
		stringstream msg;
		msg << "Resource update could not be written: " << GetError().message() << endl;
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
		stringstream msg;
		msg << "Finding resouce with id=" << resId << " in file '" << fileName << "' failed: " << GetError().message() << endl;
		throw InvalidResourceException(msg.str().c_str());
	}

	auto resSize = ::SizeofResource(dll.handle, resInfo);
	if (resSize == 0)
	{
		stringstream msg;
		msg << "Error getting size of resource with id=" << resId << " in file '" << fileName << "': " << GetError().message() << endl;
		throw InvalidResourceException(msg.str().c_str());
	}

	auto resHandle = ::LoadResource(dll.handle, resInfo);
	if (!resHandle)
	{
		stringstream msg;
		msg << "Error loading resource id=" << resId << " in file '" << fileName << "': " << GetError().message() << endl;
		throw InvalidResourceException(msg.str().c_str());
	}

	auto resData = reinterpret_cast<const char*>(::LockResource(resHandle));
	if (!resData)
	{
		stringstream msg;
		msg << "Error locking resource id=" << resId << " in file '" << fileName << "': " << GetError().message() << endl;
		throw InvalidResourceException(msg.str().c_str());
	}

	vector<unsigned char> data;
	data.reserve(resSize);
	copy(resData, resData + resSize, back_inserter(data));

	return data;
}

void ResLib::Copy(const char* fromFile, const char* resType, int fromId, /*int fromLangId, */const char* toFile, int toId/*, int toLangId*/)
{
	auto const& data = Read(fromFile, resType, fromId);
	Write(data, toFile, resType, toId);
}

