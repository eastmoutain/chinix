#include <console.h>


char buf[1024] = "123456";

int main()
{
    console_init();
    console_display_str(0, 0, 0x07, "hello, world");
    console_display_str(0, 1, 0x07, "The first program!!!");
    *buf = 1;
    return 0;
}
