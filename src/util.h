#ifndef __UTIL__
#define __UTIL__

#include <stdarg.h>

int mprintf(const char* format, ...);

void mprintf_init(void);

void mprintf_end(void);

#endif
