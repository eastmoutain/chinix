#ifndef PRINTF_H
#define PRINTF_H

#include <compiler.h>
#include <stdarg.h>
#include <stddef.h>


__BEGIN_CDECLS

typedef int (*printf_driver_func)(const char *str, size_t len);

int _printf_driver(printf_driver_func print, const char *fmt, va_list ap);

__END_CDECLS

#endif // PRINTF_H
