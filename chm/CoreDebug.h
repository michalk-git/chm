#pragma once
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

class CoreDebug
{
public:
	void CorePrintf(const char* format,...) {
		va_list arglist;
		va_start(arglist, format);
		vprintf(format, arglist);
		va_end(arglist);
	}
};

