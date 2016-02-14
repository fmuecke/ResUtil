#pragma once

// Helper to convert UTF-8 encoded std::string to std::wstring on Windows platforms
// (c) 2016, Florian Muecke

#include <Windows.h>
#include <memory>
#include <string>

namespace Utf8
{
    namespace _internal
    {
        enum class CodePage { Utf8 = CP_UTF8, Ansi = CP_ACP };

        static std::wstring to_wide(const char* str, CodePage inputCp)
        {
            static_assert(sizeof(wchar_t) == 2, "only UTF-16 wide strings supported!");

            if (!str) return std::wstring();

            auto strLen = static_cast<int>(strlen(str)); // does not include null
            auto requiredLen = ::MultiByteToWideChar(static_cast<int>(inputCp), 0, str, strLen, nullptr, 0);
            if (requiredLen == 0)
            {
                auto err = ::GetLastError();
                throw std::error_code(err, std::system_category());
            }
            auto result = std::wstring();
            result.resize(requiredLen);
            auto actualLen = ::MultiByteToWideChar(static_cast<int>(inputCp), 0, str, strLen, &result[0], requiredLen);

            if (actualLen == 0 || actualLen != requiredLen)
            {
                auto err = ::GetLastError();
                throw std::error_code(err, std::system_category());
            }

            return result;
        }

        static std::string from_wide(const wchar_t* utf16Str, CodePage outputCp)
        {
            static_assert(sizeof(wchar_t) == 2, "only UTF-16 wide strings supported!");

            if (!utf16Str) return std::string();
            auto strLen = static_cast<int>(wcslen(utf16Str)); // does not include null
            auto requiredLen = ::WideCharToMultiByte(static_cast<int>(outputCp), 0, utf16Str, strLen, nullptr, 0, nullptr, nullptr);
            if (requiredLen == 0)
            {
                auto err = ::GetLastError();
                throw std::error_code(err, std::system_category());
            }
            auto result = std::string();
            result.resize(requiredLen);
            auto actualLen = ::WideCharToMultiByte(static_cast<int>(outputCp), 0, utf16Str, strLen, &result[0], requiredLen, nullptr, nullptr);
            if (actualLen == 0)
            {
                auto err = ::GetLastError();
                throw std::error_code(err, std::system_category());
            }

            return result;
        }

    }

    static inline std::string FromWide(const wchar_t* utf16Str)
    {
        return _internal::from_wide(utf16Str, _internal::CodePage::Utf8);
    }

    static inline std::string FromWide(const std::wstring& utf16Str)
    {
        if (utf16Str.empty()) return std::string();
        return FromWide(utf16Str.c_str());
    }

    static inline std::wstring ToWide(const char* utf8Str)
    {
        return _internal::to_wide(utf8Str, _internal::CodePage::Utf8);
    }

    static inline std::wstring ToWide(const std::string& utf8Str)
    {
        if (utf8Str.empty()) return std::wstring();
        return ToWide(utf8Str.c_str());
    }

    static inline std::wstring AnsiToWide(const char* ansiStr)
    {
        return _internal::to_wide(ansiStr, _internal::CodePage::Ansi);
    }

    static inline std::string WideToAnsi(const wchar_t* utf16Str)
    {
        return _internal::from_wide(utf16Str, _internal::CodePage::Ansi);
    }
}
