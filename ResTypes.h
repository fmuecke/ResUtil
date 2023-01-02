#pragma once

#include <windows.h>
#include <WinUser.h>
#include <string>
#include <map>

namespace ResTypes
{
	static constexpr LPWSTR UNKNOWN = MAKEINTRESOURCE(0);

	namespace Strings
	{
		static const char* const Accelerator = "accelerator"; // Accelerator table.
		static const char* const Anicursor = "anicursor"; // Animated cursor.
		static const char* const Aniicon = "aniicon"; // Animated icon.
		static const char* const Bitmap = "bitmap"; // Bitmap resource.
		static const char* const Cursor = "cursor"; // Hardware - dependent cursor resource.
		static const char* const Dialog = "dialog"; // Dialog box.
		static const char* const Dlginclude = "dlginclude"; // Allows a resource editing tool to associate a string with an.rc file.Typically, the string is the name of the header file that provides symbolic names.The resource compiler parses the string but otherwise ignores the value.For example, 1 DLGINCLUDE "MyFile.h"
		static const char* const Font = "font"; // Font resource.
		static const char* const Fontdir = "fontdir"; // Font directory resource.
		static const char* const Groupcursor = "groupcursor"; // Hardware - independent cursor resource.
		static const char* const Groupicon = "groupicon"; // Hardware - independent icon resource.
		static const char* const Html = "html"; // HTML resource.
		static const char* const Icon = "icon"; // Hardware - dependent icon resource.
		static const char* const Manifest = "manifest"; // Side - by - Side Assembly Manifest.
		static const char* const Menu = "menu"; // Menu resource.
		static const char* const Messagetable = "messagetable"; // Message - table entry.
		static const char* const Plugplay = "plugplay"; // Plug and Play resource.
		static const char* const Rcdata = "rcdata"; // Application - defined resource(raw data).
		static const char* const String = "string"; // String - table entry.
		static const char* const Version = "version"; // Version resource.
		static const char* const Vxd = "vxd"; // VXD.
		static const char* const Unknown = "UNKNOWN";
	}

	static const std::map<std::string, LPWSTR> ResNameToIdMap = {
		{ Strings::Accelerator, RT_ACCELERATOR }, // Accelerator table.
		{ Strings::Anicursor, RT_ANICURSOR }, // Animated cursor.
		{ Strings::Aniicon, RT_ANIICON }, // Animated icon.
		{ Strings::Bitmap, RT_BITMAP }, // Bitmap resource.
		{ Strings::Cursor, RT_CURSOR }, // Hardware - dependent cursor resource.
		{ Strings::Dialog, RT_DIALOG }, // Dialog box.
		{ Strings::Dlginclude, RT_DLGINCLUDE }, // Allows a resource editing tool to associate a string with an.rc file.Typically, the string is the name of the header file that provides symbolic names.The resource compiler parses the string but otherwise ignores the value.For example, 1 DLGINCLUDE "MyFile.h"
		{ Strings::Font, RT_FONT }, // Font resource.
		{ Strings::Fontdir, RT_FONTDIR }, // Font directory resource.
		{ Strings::Groupcursor, RT_GROUP_CURSOR }, // Hardware - independent cursor resource.
		{ Strings::Groupicon, RT_GROUP_ICON }, // Hardware - independent icon resource.
		{ Strings::Html, RT_HTML }, // HTML resource.
		{ Strings::Icon, RT_ICON }, // Hardware - dependent icon resource.
		{ Strings::Manifest, RT_MANIFEST }, // Side - by - Side Assembly Manifest.
		{ Strings::Menu, RT_MENU }, // Menu resource.
		{ Strings::Messagetable, RT_MESSAGETABLE }, // Message - table entry.
		{ Strings::Plugplay, RT_PLUGPLAY }, // Plug and Play resource.
		{ Strings::Rcdata, RT_RCDATA }, // Application - defined resource(raw data).
		{ Strings::String, RT_STRING }, // String - table entry.
		{ Strings::Version, RT_VERSION }, // Version resource.
		{ Strings::Vxd, RT_VXD } // VXD.
	};

	static const std::map<LPWSTR, std::string> ResIdToNameMap = {
		{ RT_ACCELERATOR, Strings::Accelerator }, // Accelerator table.
		{ RT_ANICURSOR, Strings::Anicursor }, // Animated cursor.
		{ RT_ANIICON, Strings::Aniicon }, // Animated icon.
		{ RT_BITMAP, Strings::Bitmap }, // Bitmap resource.
		{ RT_CURSOR, Strings::Cursor }, // Hardware - dependent cursor resource.
		{ RT_DIALOG, Strings::Dialog }, // Dialog box.
		{ RT_DLGINCLUDE, Strings::Dlginclude }, // Allows a resource editing tool to associate a string with an.rc file.Typically, the string is the name of the header file that provides symbolic names.The resource compiler parses the string but otherwise ignores the value.For example, 1 DLGINCLUDE "MyFile.h"
		{ RT_FONT, Strings::Font }, // Font resource.
		{ RT_FONTDIR, Strings::Fontdir }, // Font directory resource.
		{ RT_GROUP_CURSOR, Strings::Groupcursor }, // Hardware - independent cursor resource.
		{ RT_GROUP_ICON, Strings::Groupicon }, // Hardware - independent icon resource.
		{ RT_HTML, Strings::Html }, // HTML resource.
		{ RT_ICON, Strings::Icon }, // Hardware - dependent icon resource.
		{ RT_MANIFEST, Strings::Manifest }, // Side - by - Side Assembly Manifest.
		{ RT_MENU, Strings::Menu }, // Menu resource.
		{ RT_MESSAGETABLE, Strings::Messagetable }, // Message - table entry.
		{ RT_PLUGPLAY, Strings::Plugplay }, // Plug and Play resource.
		{ RT_RCDATA, Strings::Rcdata }, // Application - defined resource(raw data).
		{ RT_STRING, Strings::String }, // String - table entry.
		{ RT_VERSION, Strings::Version }, // Version resource.
		{ RT_VXD, Strings::Vxd }, // VXD.
	};

	static std::string GetName(LPWSTR id)
	{
		auto const& name = ResIdToNameMap.find(id);
		if (name == ResIdToNameMap.end()) return Strings::Unknown;
		else return name->second;
	}

	static LPWSTR GetId(std::string name)
	{
		auto const& id = ResNameToIdMap.find(name);
		if (id == ResNameToIdMap.end()) return UNKNOWN;
		else return id->second;
	}
}
