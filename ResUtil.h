#pragma once
#include "Handle.hpp"
#include <iostream>
#include <vector>
#include <system_error>

class ResUtil
{
public:
	struct IoException : public std::exception
	{
		explicit IoException(const char* msg) :_msg{ msg } {}
		virtual const char* what() const override { return _msg.c_str(); }

	private:
		std::string _msg;
	};

	ResUtil();
	~ResUtil();

	static void PrintUsage()
	{
		std::cout
			<< "ReplaceRes v0.1 (c) 2015 Florian Muecke\n\n"
			<< "usage: (1) ReplaceRes.exe write /target /id /type (/langId) /rawData\n"
			<< "       (2) ReplaceRes.exe copy /target:file.ext /id:resID /type:resType (/langId:languageID) /source:file.ext /sourceId:resID (/sourceLangId:languageID)\n";
	}

	static std::vector<unsigned char> ReadData(const char* fileName)
	{
		Handle file = { ::CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) };
		if (!file.IsValid())
		{
			auto msg = std::string("Unable to open target file: ") + GetError().message();
			throw IoException(msg.c_str());
		}

		DWORD size = ::GetFileSize(file, nullptr);
		auto data = std::vector<unsigned char>(size, 0x00);
		if (0 == ReadFile(file, data.data(), static_cast<DWORD>(data.size()), &size, nullptr))
		{
			auto err = GetError();
			auto msg = std::string("Unable to read data: ") + err.message();
			throw IoException(msg.c_str());
		}

		return data;
	}

private:
	static std::error_code GetError()
	{
		return std::error_code(::GetLastError(), std::system_category());
	}

};

