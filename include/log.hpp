#pragma once
#include <WinBase.h>
#include <cstdio>
#include <cstring>
#include <cstdarg>

typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned  __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

namespace zlg
{
	inline void log(const char* func, int line, const char* format, ...)
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

		printf("[%s] [%-*.*s:% 4d] %s\n", str, 8, 8, func, line, buffer);
	}

	inline void log(const char* func, int line)
	{
		log(func, line, "%s", "");
	}
};

#define LOG(...) \
	zlg::log(__FUNCTION__, __LINE__, ##__VA_ARGS__)