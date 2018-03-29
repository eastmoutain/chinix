#include <console.h>
#include <stdio.h>
#include <assert.h>
#include <platform_timer.h>
#include <arch/interrupt.h>
#include <arch/x86.h>

int start_kernel()
{
    console_init();

    platform_init_interrupts();

    platform_init_timer();

    arch_enable_ints();

    printf("dead loop\r\n");
    printf("dead loop\r\n");
    printf("dead loop\r\n");
    for (;;) {
        asm ("nop");
    }
    debug_assert(0);

    return 0;
}
