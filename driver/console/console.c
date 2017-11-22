#include <console.h>
#include <arch/kernel_cfg.h>
#include <arch/ioport.h>

#define FRAME_BUFFER_BASE_ADDR (0xb8000UL)
#define FRAME_BUFFER_VIRT_ADDR (FRAME_BUFFER_BASE_ADDR + KERNEL_SPACE_BASE)

#define FRAME_HEIGHT    (80)
#define FRAME_WIDTH     (25)

#define ANCHOR_BUF(x,y)    (FRAME_BUFFER_VIRT_ADDR + 2*(FRAME_HEIGHT*y+x))

/* CGA */
#define CURSOR_START        0x0A
#define CURSOR_END          0x0B
#define VIDEO_ADDRESS_MSB   0x0C
#define VIDEO_ADDRESS_LSB   0x0D
#define CURSOR_POS_MSB      0x0E
#define CURSOR_POS_LSB      0x0F

struct console_viewer {
    // window attr
    struct {
        int x1, x2, y1, y2;
    } window;
    // cursor attr
    struct {
        char x, y;
        char start, end;
        char default_attr;
    } cursor;
};

struct console_viewer viewer;

static inline void console_display_char(int x, int y,
                                        unsigned char attr,
                                        unsigned char c)
{
    unsigned short *fb = (unsigned short*)ANCHOR_BUF(x, y);
    *fb = ((unsigned short)attr << 8) | c;
}

static inline void console_display_str(int x, int y,
                                        unsigned char attr,
                                        unsigned char *s)
{
    unsigned short *fb = (unsigned short*)ANCHOR_BUF(x, y);
    while (*s != '\0') {
        *fb++ = ((unsigned short)attr << 8) | *s;
    }
}
