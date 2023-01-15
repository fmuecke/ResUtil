#pragma once

#include "Handle.hpp"
#include "DataLibHandle.h"
#include "ResTypes.h"
#include "..\Utf8.hpp"

#include <Windows.h>
#include <WinUser.h>
#include <system_error>
#include <sstream>
#include <memory>
#include <iterator>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <exception>
#include <gsl/util>

namespace ResLib
{
    struct ResLibException : public std::exception
    {
        ResLibException(std::string const& msg) 
            : std::exception()
            , _msg {msg}
        {}

        const char* what() const noexcept override { return _msg.c_str(); }

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

    struct UpdateResourceException : public ResLibException
    {
        UpdateResourceException(std::string const& msg) : ResLibException(msg) {}
    };

    struct InvalidResourceException : public ResLibException
    {
        InvalidResourceException(std::string const& msg) : ResLibException(msg) {}
    };

    static void Write(std::vector<unsigned char> const& data, const char* fileName, const char* resType, const char* resId/*, int langId*/);
    static std::vector<unsigned char> Read(const char* fileName, const char* resType, const char* resId/*, int langId*/);
    static void Copy(const char* fromFile, const char* resType, const char* fromIdStr/*, int fromLangId*/, const char* toFile, const char* toIdStr/*, int toLangId*/);
    static std::vector<std::string> Enum(const char* fileName, const char* resType);
    static std::vector<std::string> EnumerateTypes(const char* fileName);
    static std::vector<int> EnumerateLanguages(const char* fileName, const char* resType);

    static std::string GetError()
    {
        const auto code = ::GetLastError();
        const auto err = std::error_code(code, std::system_category());
        const auto msg = err.message();
        return msg.empty() ? "code = " + std::to_string(err.value()) + ")\n" : msg;
    }

    static constexpr inline WORD MakeLangId(int p, int s)
    {
        return gsl::narrow_cast<WORD>(s) << 10 | gsl::narrow_cast<WORD>(p);
    }
};

// ------------------------------------------
// function definitions
// ------------------------------------------

void ResLib::Write(std::vector<unsigned char> const& data, const char* fileName, const char* resTypeStr, const char* resIdStr/*, int langId*/)
{
    if (data.empty()) throw InvalidDataException();
    if (!fileName || !resTypeStr || !resIdStr) throw ArgumentNullException();

    auto targetFileHandle = ::BeginUpdateResourceA(fileName, FALSE);
    if (Handle::IsNull(targetFileHandle))
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Opening file '" << fileName << "' failed: " << err << std::endl;
        throw InvalidFileException(msg.str().c_str());
    }

    // align data to a multiple of 8
    auto _data{ data };
    while (_data.size() % 8)
    {
        _data.emplace_back(static_cast<unsigned char>(0x00));
    }

    std::wstring customType;
    auto resType = Types::GetValue(resTypeStr, customType);
    if (resType == Types::UNDEFINED_TYPE)
    {
        resType = customType.c_str();
    }

    std::wstring customId;
    LPCWSTR resId = Types::ParseResIdString(resIdStr, customId);

    if (::UpdateResourceW(
        targetFileHandle,
        resType,
        resId,
        ResLib::MakeLangId(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        _data.data(),
        static_cast<DWORD>(data.size())) == 0)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Updating resource failed: " << err << std::endl;
        throw UpdateResourceException(msg.str().c_str());
    }
    if (::EndUpdateResourceW(targetFileHandle, false) == 0)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Resource update could not be written: " << err << std::endl;
        throw UpdateResourceException(msg.str().c_str());
    }
}

std::vector<unsigned char> ResLib::Read(const char* fileName, const char* resTypeStr, const char* resIdStr/*, int langId*/)
{
    if (!fileName || !resTypeStr || !resIdStr) throw ArgumentNullException();

    DataLibHandle dll(fileName);
    if (!dll.IsValid())
    {
        std::stringstream msg;
        msg << "Unable to open file '" << fileName << "'" << std::endl;
        throw InvalidFileException(msg.str().c_str());
    }

    std::wstring customId;
    LPCWSTR resId = Types::ParseResIdString(resIdStr, customId);

    std::wstring customType;
    auto resType = Types::GetValue(resTypeStr, customType);
    if (resType == Types::UNDEFINED_TYPE)
    {
        resType = customType.c_str();
    }

    auto resInfo = ::FindResourceW(dll, resId, resType);
    if (!resInfo)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Finding resouce with id=" << resIdStr << " in file '" << fileName << "' failed: " << err << std::endl;
        throw InvalidResourceException(msg.str().c_str());
    }

    auto resSize = ::SizeofResource(dll, resInfo);
    if (resSize == 0)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Error getting size of resource with id=" << resIdStr << " in file '" << fileName << "': " << err << std::endl;
        throw InvalidResourceException(msg.str().c_str());
    }

    auto resHandle = ::LoadResource(dll, resInfo);
    if (!resHandle)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Error loading resource id=" << resIdStr << " in file '" << fileName << "': " << err << std::endl;
        throw InvalidResourceException(msg.str().c_str());
    }

    auto pResData = reinterpret_cast<char*>(::LockResource(resHandle));
    if (!pResData)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Error locking resource id=" << resIdStr << " in file '" << fileName << "': " << err << std::endl;
        throw InvalidResourceException(msg.str().c_str());
    }

    std::vector<unsigned char> data(pResData, pResData + resSize);
    return data;
}

void ResLib::Copy(const char* fromFile, const char* resType, const char* fromIdStr, /*int fromLangId, */const char* toFile, const char* toIdStr/*, int toLangId*/)
{
    auto const& data = Read(fromFile, resType, fromIdStr);
    Write(data, toFile, resType, toIdStr);
}

std::vector<std::string> ResLib::Enum(const char* fileName, const char* resType)
{
    if (!fileName || !resType) throw ArgumentNullException();

    DataLibHandle dll(fileName);
    if (!dll.IsValid())
    {
        std::stringstream msg;
        msg << "Unable to open file '" << fileName << "'" << std::endl;
        throw InvalidFileException(msg.str().c_str());
    }

    std::wstring customType;
    auto resTypeId = Types::GetValue(resType, customType);
    if (resTypeId == Types::UNDEFINED_TYPE)
    {
        resTypeId = customType.c_str();
    }

    std::vector<std::string> data;
    
    [[gsl::suppress(bounds.1)]]
    ::EnumResourceNamesW(dll, resTypeId, 
        [](HMODULE /*hModule*/, LPCWSTR /*lpszType*/, LPWSTR lpszName, LONG_PTR lParam) -> BOOL
    {
        [[gsl::suppress(type.1)]]
        auto data = reinterpret_cast<std::vector<std::string>*>(lParam);
        if (IS_INTRESOURCE(lpszName))
        {
            data->emplace_back(std::to_string(reinterpret_cast<int>(lpszName)));
        }
        else
        {
            data->emplace_back(Utf8::FromWide(lpszName));
        }
        return true;
    }, 
        reinterpret_cast<LONG_PTR>(&data));

    return data;
}

std::vector<std::string> ResLib::EnumerateTypes(const char* fileName)
{
    if (!fileName) throw ArgumentNullException();

    DataLibHandle dll(fileName);
    if (!dll.IsValid())
    {
        std::stringstream msg;
        msg << "Unable to open file '" << fileName << "'" << std::endl;
        throw InvalidFileException(msg.str().c_str());
    }

    std::vector<std::string> types;

    [[gsl::suppress(bounds.1)]]
    ::EnumResourceTypesExW(dll, 
        [](HMODULE /*hModule*/, LPWSTR lpszType, LONG_PTR lParam) -> BOOL
        {
            auto types = reinterpret_cast<std::vector<std::string>*>(lParam);
            types->emplace_back(Types::GetName(lpszType));
            return true;
        },
            reinterpret_cast<LONG_PTR>(&types),
            RESOURCE_ENUM_LN,
            0);

    return types;
}
