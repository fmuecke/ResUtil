#pragma once

#include "LibHandle.h"

#include <Windows.h>
#include <system_error>
#include <sstream>
#include <memory>
#include <iterator>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <exception>

class ResLib
{
public:
    using Byte = unsigned char;
	
    ResLib();
	~ResLib();

    struct TypeId
    {
        static const char * const Accelerator;// Accelerator table.
        static const char * const Anicursor; // Animated cursor.
        static const char * const Aniicon; // Animated icon.
        static const char * const Bitmap; // Bitmap resource.
        static const char * const Cursor; // Hardware - dependent cursor resource.
        static const char * const Dialog; // Dialog box.
        static const char * const Dlginclude; // Allows a resource editing tool to associate a string with an.rc file.Typically, the string is the name of the header file that provides symbolic names.The resource compiler parses the string but otherwise ignores the value.For example, 1 DLGINCLUDE "MyFile.h"
        static const char * const Font; // Font resource.
        static const char * const Fontdir; // Font directory resource.
        static const char * const Groupcursor; // Hardware - independent cursor resource.
        static const char * const Groupicon; // Hardware - independent icon resource.
        static const char * const Html; // HTML resource.
        static const char * const Icon; // Hardware - dependent icon resource.
        static const char * const Manifest; // Side - by - Side Assembly Manifest.
        static const char * const Menu; // Menu resource.
        static const char * const Messagetable; // Message - table entry.
        static const char * const Plugplay; // Plug and Play resource.
        static const char * const Rcdata; // Application - defined resource(raw data).
        static const char * const String; // String - table entry.
        static const char * const Version; // Version resource.
        static const char * const Vxd; // VXD.
    };

	struct ResLibException : public std::exception
	{
		ResLibException(std::string const& msg) 
			: std::exception()
			, _msg {msg}
		{}

		virtual const char* what() const override { return _msg.c_str(); }

	private:
		std::string _msg;
	};

	struct InvalidArgsException : public std::exception {};
	struct ArgumentNullException : public std::exception {};
	struct InvalidDataException : public std::exception {};

	struct InvalidFileException : public ResLibException
	{
		InvalidFileException(std::string const& msg) : ResLibException(msg) {}
	};

	struct InvalidTypeException : public ResLibException
	{
		InvalidTypeException(const char* type) : ResLibException(std::string("The type is not valid: ").append(type)) {}
	};

	struct UpdateResourceException : public ResLibException
	{
		UpdateResourceException(std::string const& msg) : ResLibException(msg) {}
	};

	struct InvalidResourceException : public ResLibException
	{
		InvalidResourceException(std::string const& msg) : ResLibException(msg) {}
	};

	static void Write(std::vector<char> const& data, const char* fileName, const char* resType, int resId/*, int langId*/);
	static std::vector<char> Read(const char* fileName, const char* resType, int resId/*, int langId*/);
	static void Copy(const char* fromFile, const char* resType, int fromId/*, int fromLangId*/, const char* toFile, int toId/*, int toLangId*/);
	static const std::map<const std::string, const char*> Types;

private:
    static std::string GetError()
    {
        auto code = ::GetLastError();
        auto err = std::error_code(code, std::system_category());
        auto msg = err.message();
        return msg.empty() ? "code = " + std::to_string(err.value()) + ")\n" : msg;
    }

    static constexpr inline const char* IntResA(int i)
    {
        [[suppress(type.1)]] return reinterpret_cast<const char*>(i);
    }

    static constexpr inline WORD MakeLangId(int p, int s)
    {
        return static_cast<WORD>(s) << 10 | static_cast<WORD>(p);
    }
};

const char * const ResLib::TypeId::Accelerator = "accelerator"; // Accelerator table.
const char * const ResLib::TypeId::Anicursor = "anicursor"; // Animated cursor.
const char * const ResLib::TypeId::Aniicon = "aniicon"; // Animated icon.
const char * const ResLib::TypeId::Bitmap = "bitmap"; // Bitmap resource.
const char * const ResLib::TypeId::Cursor = "cursor"; // Hardware - dependent cursor resource.
const char * const ResLib::TypeId::Dialog = "dialog"; // Dialog box.
const char * const ResLib::TypeId::Dlginclude = "dlginclude"; // Allows a resource editing tool to associate a string with an.rc file.Typically, the string is the name of the header file that provides symbolic names.The resource compiler parses the string but otherwise ignores the value.For example, 1 DLGINCLUDE "MyFile.h"
const char * const ResLib::TypeId::Font = "font"; // Font resource.
const char * const ResLib::TypeId::Fontdir = "fontdir"; // Font directory resource.
const char * const ResLib::TypeId::Groupcursor = "groupcursor"; // Hardware - independent cursor resource.
const char * const ResLib::TypeId::Groupicon = "groupicon"; // Hardware - independent icon resource.
const char * const ResLib::TypeId::Html = "html"; // HTML resource.
const char * const ResLib::TypeId::Icon = "icon"; // Hardware - dependent icon resource.
const char * const ResLib::TypeId::Manifest = "manifest"; // Side - by - Side Assembly Manifest.
const char * const ResLib::TypeId::Menu = "menu"; // Menu resource.
const char * const ResLib::TypeId::Messagetable = "messagetable"; // Message - table entry.
const char * const ResLib::TypeId::Plugplay = "plugplay"; // Plug and Play resource.
const char * const ResLib::TypeId::Rcdata = "rcdata"; // Application - defined resource(raw data).
const char * const ResLib::TypeId::String = "string"; // String - table entry.
const char * const ResLib::TypeId::Version = "version"; // Version resource.
const char * const ResLib::TypeId::Vxd = "vxd"; // VXD.

[[suppress(type.4), suppress(bounds.1)]]
const std::map<const std::string, const char*> ResLib::Types =
{
    { TypeId::Accelerator, IntResA(9) }, // Accelerator table.
    { TypeId::Anicursor, IntResA(21) }, // Animated cursor.
    { TypeId::Aniicon, IntResA(22) }, // Animated icon.
    { TypeId::Bitmap, IntResA(2) }, // Bitmap resource.
    { TypeId::Cursor, IntResA(1) }, // Hardware - dependent cursor resource.
    { TypeId::Dialog, IntResA(5) }, // Dialog box.
    { TypeId::Dlginclude, IntResA(17) }, // Allows a resource editing tool to associate a string with an.rc file.Typically, the string is the name of the header file that provides symbolic names.The resource compiler parses the string but otherwise ignores the value.For example, 1 DLGINCLUDE "MyFile.h"
    { TypeId::Font, IntResA(8) }, // Font resource.
    { TypeId::Fontdir, IntResA(7) }, // Font directory resource.
    { TypeId::Groupcursor, IntResA(12) }, // Hardware - independent cursor resource.
    { TypeId::Groupicon, IntResA(14) }, // Hardware - independent icon resource.
    { TypeId::Html, IntResA(23) }, // HTML resource.
    { TypeId::Icon, IntResA(3) }, // Hardware - dependent icon resource.
    { TypeId::Manifest, IntResA(24) }, // Side - by - Side Assembly Manifest.
    { TypeId::Menu, IntResA(4) }, // Menu resource.
    { TypeId::Messagetable, IntResA(11) }, // Message - table entry.
    { TypeId::Plugplay, IntResA(19) }, // Plug and Play resource.
    { TypeId::Rcdata, IntResA(10) }, // Application - defined resource(raw data).
    { TypeId::String, IntResA(6) }, // String - table entry.
    { TypeId::Version, IntResA(16) }, // Version resource.
    { TypeId::Vxd, IntResA(20) } // VXD.
};

ResLib::ResLib()
{}

ResLib::~ResLib()
{}

void ResLib::Write(std::vector<char> const& data, const char* fileName, const char* resType, int resId/*, int langId*/)
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
        std::stringstream msg;
        msg << "Opening file '" << fileName << "' failed: " << err << std::endl;
        throw InvalidFileException(msg.str().c_str());
    }

    auto _data = data;

    while (_data.size() % 8) _data.emplace_back(0x00);  // data must be aligned

    if (::UpdateResourceA(
        targetFileHandle,
        resTypePos->second,
        IntResA(resId),
        ResLib::MakeLangId(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        _data.data(),
        static_cast<DWORD>(data.size())) == 0)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Updating resource failed: " << err << std::endl;
        throw UpdateResourceException(msg.str().c_str());
    }
    if (::EndUpdateResourceA(targetFileHandle, false) == 0)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Resource update could not be written: " << err << std::endl;
        throw UpdateResourceException(msg.str().c_str());
    }
}

std::vector<char> ResLib::Read(const char* fileName, const char* resType, int resId/*, int langId*/)
{
    if (!fileName | !resType) throw ArgumentNullException();
    auto resTypePos = Types.find(resType);
    if (resTypePos == Types.end()) throw InvalidTypeException(resType);
    if (resId < 0 /*|| langId < 0*/) throw InvalidArgsException();

    auto dll = LibHandle(fileName);
    if (!dll.IsValid())
    {
        std::stringstream msg;
        msg << "Unable to open file '" << fileName << std::endl;
        throw InvalidFileException(msg.str().c_str());
    }

    auto resInfo = ::FindResourceA(dll.handle, MAKEINTRESOURCEA(resId), resTypePos->second);
    if (!resInfo)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Finding resouce with id=" << resId << " in file '" << fileName << "' failed: " << err << std::endl;
        throw InvalidResourceException(msg.str().c_str());
    }

    auto resSize = ::SizeofResource(dll.handle, resInfo);
    if (resSize == 0)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Error getting size of resource with id=" << resId << " in file '" << fileName << "': " << err << std::endl;
        throw InvalidResourceException(msg.str().c_str());
    }

    auto resHandle = ::LoadResource(dll.handle, resInfo);
    if (!resHandle)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Error loading resource id=" << resId << " in file '" << fileName << "': " << err << std::endl;
        throw InvalidResourceException(msg.str().c_str());
    }

    [[suppress(type.1)]]
    auto pResData = reinterpret_cast<char*>(::LockResource(resHandle));
    if (!pResData)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Error locking resource id=" << resId << " in file '" << fileName << "': " << err << std::endl;
        throw InvalidResourceException(msg.str().c_str());
    }

    std::vector<char> data(pResData, pResData + resSize);
    return data;
}

void ResLib::Copy(const char* fromFile, const char* resType, int fromId, /*int fromLangId, */const char* toFile, int toId/*, int toLangId*/)
{
    auto const& data = Read(fromFile, resType, fromId);
    Write(data, toFile, resType, toId);
}

