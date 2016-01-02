#pragma once

#include <vector>
#include <map>
#include <string>
#include <exception>

class ResLib
{
public:
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


	static void Write(std::vector<unsigned char> const& data, const char* fileName, const char* resType, int resId/*, int langId*/);
	static std::vector<unsigned char> Read(const char* fileName, const char* resType, int resId/*, int langId*/);
	static void Copy(const char* fromFile, const char* resType, int fromId/*, int fromLangId*/, const char* toFile, int toId/*, int toLangId*/);
	static const std::map<const std::string, const char*> Types;
};

