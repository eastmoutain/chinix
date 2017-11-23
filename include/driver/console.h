#ifndef CONSOLE_H
#define CONSOLE_H


void console_display_str(int x, int y,
                         unsigned char attr,
                         unsigned char *s);

void console_init(void);

#endif // CONSOLE_H

