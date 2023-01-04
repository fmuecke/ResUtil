#pragma once

#include <string>
#include <map>

namespace StringHelper
{

    template<typename InputIt>
    std::string join(InputIt begin, InputIt end, const std::string& separator = ", ")
    {
        std::ostringstream ss;

        if (begin != end) { ss << *begin++; }

        while (begin != end)
        {
            ss << separator;
            ss << *begin++;
        }

        return ss.str();
    }

    template<typename container>
    std::string join(container c, const std::string& separator = ", ")
    {
        auto begin = c.begin();
        auto end = c.end();

        return join(begin, end, separator);
    }

    template<typename valueType, typename keyType>
    std::string join(std::map<valueType, keyType> m, const std::string& separator = ", ")
    {
        auto begin = m.begin();
        auto end = m.end();

        std::ostringstream ss;

        if (begin != end) 
        { 
            ss << begin->first;
            ++begin;
        }

        while (begin != end)
        {
            ss << separator;
            ss << begin->first;
            ++begin;
        }

        return ss.str();
    }
}