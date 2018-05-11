#ifndef STDIO_H
#define STDIO_H

#include <printf.h>
#include <kernel/thread.h>

#define printf(x...) \
    do { \
        thread_preempt_disable();\
        _printf(x); \
        thread_preempt_enable(); \
    } while (0)

int _vfprintf(const char *fmt, va_list ap);

#endif // STDIO_H
