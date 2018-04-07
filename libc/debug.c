#include <debug.h>
#include <string.h>
#include <stdio.h>
#include <arch/x86.h>

static void platform_halt(void)
{
    for (;;) {
        x64_cli();
        x64_hlt();
    }
}

void platform_panic(const char *fmt, ...)
{
    va_list(ap);
    va_start(ap, fmt);
    _vfprintf(fmt, ap);
    va_end(ap);

    platform_halt();
}
