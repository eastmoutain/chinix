#include <console.h>
#include <stdio.h>

int start_kernel()
{
    console_init();

    printf("hello world!\n");

    return 0;
}
