#pragma once
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

class CoreDebug
{
	FILE* f;
public:
	void OpenFile(const char* file_name) {
		f = fopen(file_name, "w");
	}
	void CorePrintf(const char* format,...) {                        //check if f is null?
		va_list arglist;
		va_start(arglist, format);
		vprintf(format, arglist);
		va_end(arglist);
	}
	void CloseFile() {
		fclose(f);
	}
};

