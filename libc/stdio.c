#include <stdio.h>
#include <stdarg.h>
#include <printf.h>


int print(const char *str, size_t len)
{

}

int vfprintf(const char *fmt, va_list ap)
{
    _printf_driver(print, fmt, ap);
}



int _printf(const char *fmt, ...)
{
    va_list ap;
    int err;

    va_start(ap, fmt);
    err = vfprintf(fmt, ap);
    va_end(ap);

    return err;
}
