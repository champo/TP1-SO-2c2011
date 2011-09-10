#ifndef __UTIL__
#define __UTIL__

#include <stdarg.h>

int mprintf(const char* format, ...);

int mprintf_init(void);

void mprintf_end(void);

void print_trace(void);

void print_error(const char* format, ...);

void print_errno(const char* tag);

#endif
