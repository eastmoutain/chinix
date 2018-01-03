#include <console.h>
#include <arch/kernel_cfg.h>
#include <arch/ioport.h>
#include <arch/x86.h>

#define FRAME_BUFFER_BASE_ADDR (0xb8000UL)
#define FRAME_BUFFER_VIRT_ADDR (FRAME_BUFFER_BASE_ADDR + KERNEL_SPACE_BASE)

#define FRAME_WIDTH    (80)
#define FRAME_HEIGHT   (25)

#define ANCHOR_BUF(x,y)    (FRAME_BUFFER_VIRT_ADDR + 2*(FRAME_WIDTH*y+x))

/* CGA */
#define CURSOR_START        0x0A
#define CURSOR_END          0x0B
#define VIDEO_ADDRESS_MSB   0x0C
#define VIDEO_ADDRESS_LSB   0x0D
#define CURSOR_POS_MSB      0x0E
#define CURSOR_POS_LSB      0x0F

struct window {
    int x1, x2, y1, y2;
};
struct cursor {
    char x, y;
    char start, end;
    char default_attr;
};

struct console_viewer {
    // window attr
    struct window wd;
    // cursor attr
    struct cursor cs;
};

struct console_viewer viewer;

void place(struct cursor *cs, int x, int y)
{
    unsigned short word = x + y*80;

    outb(CGA_INDEX_PORT, CURSOR_POS_LSB);
    outb(CGA_DATA_PORT, word & 0xff);
    outb(CGA_INDEX_PORT, CURSOR_POS_MSB);
    outb(CGA_DATA_PORT, (word >> 8) & 0xff);
    cs->x = x;
    cs->y = y;
}

static inline unsigned short read_pos_value(int x, int y)
{
    return *(unsigned short*)ANCHOR_BUF(x, y);
}

static inline void set_pos_value(int x, int y, unsigned short value)
{
    unsigned short *fb = (unsigned short*)ANCHOR_BUF(x, y);
    *fb = value; 
}

static inline void console_display_char(int x, int y,
                                        unsigned char attr,
                                        unsigned char c)
{
    unsigned short *fb = (unsigned short*)ANCHOR_BUF(x, y);
    *fb = ((unsigned short)attr << 8) | c;
}

void console_display_str(int x, int y,
                                        unsigned char attr,
                                        unsigned char *s)
{
    unsigned short *fb = (unsigned short*)ANCHOR_BUF(x, y);
    while (*s != '\0') {
        *fb++ = ((unsigned short)attr << 8) | *s++;
    }
}

void clear_console(struct console_viewer *cv)
{
    int x, y;
    int width = cv->wd.x2;
    int height = cv->wd.y2;
    char c = ' ';
    char attr = cv->cs.default_attr;

    for(y = cv->wd.y1; y <= height; y++) {
        for (x = cv->wd.x1; x <= width; x++) {
            console_display_char(x, y, attr, c);
        }
    }

    cv->cs.x = cv->wd.x1;
    cv->cs.y = cv->wd.y1;
    return;
}

void scroll(struct console_viewer *vw)
{
    int x, y;
    struct window *wd = &vw->wd;
    struct cursor *cs = &vw->cs;
    unsigned char default_attr = cs->default_attr;
    unsigned short value;
    
    for (y = wd->y1 + 1; y <= wd->y2; y++) {
        for (x = wd->x1; x <= wd->x2; x++) {
           value  = read_pos_value(x, y);
           set_pos_value(x, y-1, value);
        }
    }
    
    for (x = wd->x1; x <= wd->x2; x++) {
        set_pos_value(x, wd->y2, default_attr << 8);
    }
}

void console_putc(char c)
{
    struct console_viewer *vw = &viewer;
    struct cursor *cs = &vw->cs;
    struct window *wd = &vw->wd;
    int x = cs->x;
    int y = cs->y;

    switch (c) {
        case '\t':
            x += 8;
            if (x >= wd->x2 + 1) {
                x = wd->x1;
                if (y == wd->y2) {
                    scroll(vw);
                } else {
                    y++;
                }
            } else {
                // round up to 8byte upper boarder
                x = ((x + 0x07) & 0xf8);
            }
            break;
        case '\r':
            x = wd->x1;
            break;
        case '\n':
            if (y == wd->y2) {
                scroll(vw);
            } else {
                y++;
            }
            break;
        case '\b':
            x--;
            console_display_char(x, y, cs->default_attr, ' ');
            break;
        default:
            console_display_char(x, y, cs->default_attr, c);
            x++;
            if (x >= wd->x2 + 1) {
                x = wd->x1;
                if (y == wd->y2) {
                    scroll(vw);
                } else {
                    y++;
                }
            }
            break;
    }

    place(cs, x, y);
}

int console_put_str(const char *str, size_t len)
{
    int i;

    for (i = 0; i < len; i++) {
        if (str[i] == '\n')
            console_putc('\r');

        console_putc(str[i]);
    }

    return len;
}

void console_init(void)
{
    viewer.wd.x1 = 0;
    viewer.wd.x2 = FRAME_WIDTH - 1;
    viewer.wd.y1 = 0;
    viewer.wd.y2 = FRAME_HEIGHT - 1;
    viewer.cs.x = 0;
    viewer.cs.y = 0;
    viewer.cs.default_attr = 0x07;
    clear_console(&viewer);
    place(&viewer.cs, 0, 0);
}
