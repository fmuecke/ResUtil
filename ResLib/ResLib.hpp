#pragma once

#include "DataLibHandle.h"
#include "..\Utf8.hpp"
#include "..\ResTypes.h"

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


class ResLib
{
public:
    using Byte = unsigned char;
    
    ResLib() = default;

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

    static void Write(std::vector<char> const& data, const char* fileName, const char* resType, int resId/*, int langId*/);
    static std::vector<char> Read(const char* fileName, const char* resType, int resId/*, int langId*/);
    static void Copy(const char* fromFile, const char* resType, int fromId/*, int fromLangId*/, const char* toFile, int toId/*, int toLangId*/);
    static std::vector<std::string> Enum(const char* fileName, const char* resType);
    static std::vector<std::string> EnumerateTypes(const char* fileName);
    static std::vector<int> EnumerateLanguages(const char* fileName, const char* resType);

private:
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

void ResLib::Write(std::vector<char> const& data, const char* fileName, const char* resType, int resId/*, int langId*/)
{
    if (data.empty()) throw InvalidDataException();
    if (!fileName || !resType) throw ArgumentNullException();
    if (resId < 0 /*|| langId < 0*/) throw InvalidArgsException();

    auto targetFileHandle = ::BeginUpdateResourceA(fileName, FALSE);
    if (Handle::IsNull(targetFileHandle))
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Opening file '" << fileName << "' failed: " << err << std::endl;
        throw InvalidFileException(msg.str().c_str());
    }

    auto _data = data;
    while (_data.size() % 8) _data.emplace_back(0x00);  // data must be aligned

    std::wstring customType;
    auto resTypeId = ResTypes::GetId(resType, customType);
    if (resTypeId == ResTypes::UNDEFINED_TYPE)
    {
        resTypeId = customType.c_str();
    }

    if (::UpdateResourceW(
        targetFileHandle,
        resTypeId,
        MAKEINTRESOURCEW(resId),
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

std::vector<char> ResLib::Read(const char* fileName, const char* resType, int resId/*, int langId*/)
{
    if (!fileName || !resType) throw ArgumentNullException();
    if (resId < 0 /*|| langId < 0*/) throw InvalidArgsException();

    DataLibHandle dll(fileName);
    if (!dll.IsValid())
    {
        std::stringstream msg;
        msg << "Unable to open file '" << fileName << "'" << std::endl;
        throw InvalidFileException(msg.str().c_str());
    }

    std::wstring customType;
    auto resTypeId = ResTypes::GetId(resType, customType);
    if (resTypeId == ResTypes::UNDEFINED_TYPE)
    {
        resTypeId = customType.c_str();
    }

    auto resInfo = ::FindResourceW(dll, MAKEINTRESOURCEW(resId), resTypeId);
    if (!resInfo)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Finding resouce with id=" << resId << " in file '" << fileName << "' failed: " << err << std::endl;
        throw InvalidResourceException(msg.str().c_str());
    }

    auto resSize = ::SizeofResource(dll, resInfo);
    if (resSize == 0)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Error getting size of resource with id=" << resId << " in file '" << fileName << "': " << err << std::endl;
        throw InvalidResourceException(msg.str().c_str());
    }

    auto resHandle = ::LoadResource(dll, resInfo);
    if (!resHandle)
    {
        auto err = GetError();
        std::stringstream msg;
        msg << "Error loading resource id=" << resId << " in file '" << fileName << "': " << err << std::endl;
        throw InvalidResourceException(msg.str().c_str());
    }

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
    auto resTypeId = ResTypes::GetId(resType, customType);
    if (resTypeId == ResTypes::UNDEFINED_TYPE)
    {
        resTypeId = customType.c_str();
    }

    std::vector<std::string> data;
    
    [[gsl::suppress(bounds.1)]]
    ::EnumResourceNamesW(dll, resTypeId, 
        [](HMODULE /*hModule*/, LPCWSTR lpszType, LPWSTR lpszName, LONG_PTR lParam) -> BOOL
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
            if (IS_INTRESOURCE(lpszType))
            {
                types->emplace_back(ResTypes::GetName(lpszType));
            }
            else
            {
                types->emplace_back('\"' + Utf8::FromWide(lpszType) + '\"');
            }
            return true;
        },
            reinterpret_cast<LONG_PTR>(&types),
            RESOURCE_ENUM_LN,
            0);

    return types;
}
