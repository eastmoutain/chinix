#include <stdio.h>
#include <stdarg.h>
#include <printf.h>
#include <console.h>

int _vfprintf(const char *fmt, va_list ap)
{
    return _printf_driver(console_put_str, fmt, ap);
}

int _printf(const char *fmt, ...)
{
    va_list ap;
    int err;

    va_start(ap, fmt);
    err = _vfprintf(fmt, ap);
    va_end(ap);

    return err;
}

