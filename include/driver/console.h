#ifndef CONSOLE_H
#define CONSOLE_H

#include <stddef.h>

void console_display_str(int x, int y,
                         unsigned char attr,
                         unsigned char *s);

int console_put_str(const char *str, size_t len);

void console_init(void);

#endif // CONSOLE_H

