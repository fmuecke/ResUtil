#pragma once

#include <vector>
#include <map>
#include <string>
#include <exception>
//#include <WinUser.h>

class ResLib
{
public:
	ResLib();
	~ResLib();

	struct InvalidFileException : std::exception
	{
		InvalidFileException(const char* msg) : std::exception(msg)	{}
	};

	struct InvalidArgsException : std::exception 
	{
	};

	struct ArgumentNullException : std::exception
	{
	};

	struct InvalidTypeException : std::exception
	{
		InvalidTypeException(const char* type) : std::exception(std::string("The type is not valid: ").append(type).c_str())
		{}
	};

	struct UpdateResourceException : std::exception
	{
		UpdateResourceException(const char* msg) : std::exception(msg) {}
	};

	struct InvalidDataException : std::exception
	{

	};

	static const std::map<const std::string, const char*> Types;

	static void Write(std::vector<char> const& data, const char* targetFile, const char* resType, int resId, int langId);
	static void Clone(const char* fromFile, const char* resType, int fromId, int fromLangId, const char* toFile, int toId, int toLangId);
	static std::vector<char> Read(const char* targetFile, const char* resType, int resId, int langId);
};

