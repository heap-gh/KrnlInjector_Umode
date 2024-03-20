#pragma once


#include <TlHelp32.h>
#include <Windows.h>
#include <strsafe.h>
#include <string>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

namespace helper
{

    static wchar_t* convertToWideChar(const char* str)
    {
        if (str == nullptr)
            return nullptr;

        // Determine the length of the wide char string
        size_t len = mbstowcs(nullptr, str, 0);
        if (len == static_cast<size_t>(-1))
        {
            // Error handling if conversion fails
            std::cerr << "Error occurred while converting string." << std::endl;
            return nullptr;
        }

        // Allocate memory for the wide char string
        wchar_t* wstr = new wchar_t[len + 1]; // +1 for null terminator
        mbstowcs(wstr, str, len + 1); // Convert the string

        return wstr;
    }


    static std::string wstringToString(const std::wstring& wstr)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        return converter.to_bytes(wstr);
    }


    bool isNumber(const std::string& str)
    {
        for (char c : str)
        {
            if (!std::isdigit(c))
            {
                return false;
            }
        }
        return true;
    }


}


