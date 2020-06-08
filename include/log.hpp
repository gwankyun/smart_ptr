#pragma once
#ifdef _MSC_VER
#include <Windows.h>
#endif // _MSC_VER
#include <cstdio>
#include <cstring>
#include <cstdarg> // va_list va_start va_end
#include <string>
#include <cstdint> // uint_least32_t
#include "compile_features.h" // CXX_NOEXCEPT

#ifdef __has_include
#if __has_include(<source_location>)
#include <source_location>
#ifndef STD_SOURCE_LOCATION
#define STD_SOURCE_LOCATION
#endif // !STD_SOURCE_LOCATION
#endif
#endif

#ifndef NOEXCEPT
#ifdef CXX_NOEXCEPT
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif // CXX_NOEXCEPT
#endif // !NOEXCEPT

namespace lightweight
{
#ifndef UINT_LEAST32_MAX
    typedef unsigned int uint_least32_t;
#endif // !UINT_LEAST32_MAX

    struct source_location
    {
        source_location() NOEXCEPT
            : _line(0), _column(0), _file_name(""), _function_name("")
        {

        }

        source_location(const source_location& other)
            : _line(other.line()), _column(other.column()), _file_name(other.file_name()), _function_name(other.function_name())
        {

        }

#ifdef __cpp_rvalue_references
        source_location(source_location&& other) NOEXCEPT
            : _line(other.line()), _column(other.column()), _file_name(other.file_name()), _function_name(other.function_name())
        {

        }
#endif // __cpp_rvalue_references

        source_location(uint_least32_t line, uint_least32_t column, const std::string& file_name, const std::string& function_name)
            : _line(line), _column(column), _file_name(file_name), _function_name(function_name)
        {

        }

        ~source_location()
        {

        }

        inline uint_least32_t line() const
        {
            return _line;
        }

        inline uint_least32_t column() const
        {
            return _column;
        }

        inline const char* file_name() const
        {
            return _file_name.c_str();
        }

        inline const char* function_name() const
        {
            return _function_name.c_str();
        }

    private:
        uint_least32_t _line;
        uint_least32_t _column;
        std::string _file_name;
        std::string _function_name;
    };

    inline void log(const source_location& src, const char* format, ...)
    {
        SYSTEMTIME localTime;
        GetLocalTime(&localTime);
        char str[128];
        memset(str, '\0', sizeof(str));
        sprintf_s(str, sizeof(str), "%d-%02d-%02d %02d:%02d:%02d.%03d",
            localTime.wYear, localTime.wMonth, localTime.wDay,
            localTime.wHour, localTime.wMinute, localTime.wSecond, localTime.wMilliseconds);

        char buffer[256];
        memset(buffer, '\0', sizeof(buffer));

        va_list ap;
        va_start(ap, format);
        vsprintf_s(buffer, sizeof(buffer), format, ap);
        va_end(ap);

        printf("[%s] [%-*.*s:% 4d] %s\n", str, 15, 15, src.function_name(), src.line(), buffer);
    }

    inline void log(const source_location& src)
    {
        log(src, "%s", "");
    }

#ifdef STD_SOURCE_LOCATION
    inline void log(const std::source_location& src, const char* format, ...)
    {
        SYSTEMTIME localTime;
        GetLocalTime(&localTime);
        char str[128];
        memset(str, '\0', sizeof(str));
        sprintf_s(str, sizeof(str), "%d-%02d-%02d %02d:%02d:%02d.%03d",
            localTime.wYear, localTime.wMonth, localTime.wDay,
            localTime.wHour, localTime.wMinute, localTime.wSecond, localTime.wMilliseconds);

        char buffer[256];
        memset(buffer, '\0', sizeof(buffer));

        va_list ap;
        va_start(ap, format);
        vsprintf_s(buffer, sizeof(buffer), format, ap);
        va_end(ap);

        printf("[%s] [%-*.*s:% 4d] %s\n", str, 15, 15, src.function_name(), src.line(), buffer);
    }

    inline void log(const std::source_location& src)
    {
        log(src, "%s", "");
    }
#endif // STD_SOURCE_LOCATION

};

#ifdef STD_SOURCE_LOCATION
#define LOG(...) \
	lightweight::log(std::source_location::current(), ##__VA_ARGS__)
#else
#define LOG(...) \
	lightweight::log(lightweight::source_location(__LINE__, 0, __FILE__, __func__), ##__VA_ARGS__)
#endif // STD_SOURCE_LOCATION