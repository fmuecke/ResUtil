#pragma once
#include "Handle.hpp"
#include "Utf8.hpp"
#include <iostream>
#include <vector>
#include <system_error>
#include <gsl/util>

class ResUtil
{
public:
	struct IoException : public std::exception
	{
		explicit IoException(const char* msg) :_msg{ msg } {}
		const char* what() const noexcept override { return _msg.c_str(); }

	private:
		std::string _msg;
	};

	ResUtil() noexcept;

	static std::vector<char> ReadData(const char* fileName)
	{
		Handle file = { ::CreateFileW(Utf8::ToWide(fileName).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr) };
		if (!file.IsValid())
		{
			const auto msg = std::string("Unable to open target file: ") + GetError().message();
			throw IoException(msg.c_str());
		}

		DWORD size = ::GetFileSize(file, nullptr);
		auto data = std::vector<char>(size, 0x00);
		if (0 == ::ReadFile(file, data.data(), gsl::narrow_cast<DWORD>(data.size()), &size, nullptr))
		{
			const auto err = GetError();
			const auto msg = std::string("Unable to read data: ") + err.message();
			throw IoException(msg.c_str());
		}

		return data;
	}

	static void WriteData(std::vector<char> const& data, const char* fileName)
	{
		Handle file = { ::CreateFileW(Utf8::ToWide(fileName).c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr) };
		DWORD bytesWritten{ 0 };
		if (!::WriteFile(file, data.data(), gsl::narrow_cast<DWORD>(data.size()), &bytesWritten, nullptr))
		{
			const auto err = GetError();
			const auto msg = std::string("Unable to write data: ") + err.message();
			throw IoException(msg.c_str());
		}
	}

private:
	static std::error_code GetError() noexcept
	{
		return std::error_code(::GetLastError(), std::system_category());
	}
};

