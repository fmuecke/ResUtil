#pragma once
#include "Handle.hpp"
#include "Utf8.hpp"
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

	static std::vector<char> ReadData(const char* fileName)
	{
		Handle file = { ::CreateFileW(Utf8::ToWide(fileName).c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) };
		if (!file.IsValid())
		{
			auto msg = std::string("Unable to open target file: ") + GetError().message();
			throw IoException(msg.c_str());
		}

		DWORD size = ::GetFileSize(file, nullptr);
		auto data = std::vector<char>(size, 0x00);
		if (0 == ::ReadFile(file, data.data(), static_cast<DWORD>(data.size()), &size, nullptr))
		{
			auto err = GetError();
			auto msg = std::string("Unable to read data: ") + err.message();
			throw IoException(msg.c_str());
		}

		return data;
	}

	static void WriteData(std::vector<char> const& data, const char* fileName)
	{
		Handle file = { ::CreateFileW(Utf8::ToWide(fileName).c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) };
		DWORD bytesWritten{ 0 };
		if (!::WriteFile(file, data.data(), static_cast<DWORD>(data.size()), &bytesWritten, 0))
		{
			auto err = GetError();
			auto msg = std::string("Unable to write data: ") + err.message();
			throw IoException(msg.c_str());
		}
	}

private:
	static std::error_code GetError()
	{
		return std::error_code(::GetLastError(), std::system_category());
	}
};

