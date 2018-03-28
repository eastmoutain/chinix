#ifndef STDIO_H
#define STDIO_H

#include <printf.h>

#define printf(x...) _printf(x)

int vfprintf(const char *fmt, va_list ap);

#endif // STDIO_H
