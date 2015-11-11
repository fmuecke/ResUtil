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

	struct ResLibException : public std::exception
	{
		ResLibException(std::string const& msg) 
			: std::exception()
			, _msg {msg}
		{}

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

	static const std::map<const std::string, const char*> Types;

	static void Write(std::vector<unsigned char> const& data, const char* fileName, const char* resType, int resId/*, int langId*/);
	static std::vector<unsigned char> Read(const char* fileName, const char* resType, int resId/*, int langId*/);
	static void Copy(const char* fromFile, const char* resType, int fromId/*, int fromLangId*/, const char* toFile, int toId/*, int toLangId*/);
};

