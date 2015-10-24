#include "stdafx.h"
#include "ResLib.h"
#include <Windows.h>
#include <system_error>
#include <sstream>
//#include "../Handle.hpp"

using namespace std;

error_code GetError()
{
	return error_code(::GetLastError(), system_category());
}

//vector<char> ReadData(const char* fileName)
//{
//	Handle file = { ::CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) };
//	if (!file.IsValid())
//	{
//		auto err = GetError();
//		auto msg = string("Unable to open target file: ") + err.message();
//		throw std::exception(msg.c_str(), err.value());
//	}
//
//	DWORD size = ::GetFileSize(file, nullptr);
//	vector<char> data(size, 0x00);
//	if (0 == ReadFile(file, data.data(), static_cast<DWORD>(data.size()), &size, nullptr))
//	{
//		auto err = GetError();
//		auto msg = string("Unable to read data: ") + err.message();
//		throw std::exception(msg.c_str(), err.value());
//	}
//
//	return data;
//}

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

void ResLib::Write(std::vector<char> const& data, const char* targetFile, const char* resType, int resId, int langId)
{
	if (data.empty()) throw InvalidDataException();
	if (!targetFile | !resType) throw ArgumentNullException();
	if (resId < 0 || langId < 0) throw InvalidArgsException();
	auto typePos = Types.find(resType);
	if (typePos == Types.end()) throw InvalidTypeException(resType);

	auto targetFileHandle = ::BeginUpdateResourceA(targetFile, FALSE);
	if (targetFileHandle == NULL)
	{
		auto err = GetError();
		stringstream msg;
		msg << "Opening file '" << targetFile << L"' failed: "
		    << err.message() << endl;
		throw InvalidFileException(msg.str().c_str());
	}

	auto _data = data;

	while (_data.size() % 4) _data.push_back(0x00);  // data must be aligned

	if (::UpdateResourceA(targetFileHandle, (*typePos).second, MAKEINTRESOURCEA(resId), langId, _data.data(), static_cast<DWORD>(data.size())) == 0)
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

void ResLib::Clone(const char* fromFile, const char* resType, int fromId, int fromLangId, const char* toFile, int toId, int toLangId)
{

}

std::vector<char> ResLib::Read(const char* targetFile, const char* resType, int resId, int langId)
{
	return{};
}
