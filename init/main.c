#include <console.h>
#include <stdio.h>
#include <assert.h>
#include <platform_timer.h>
#include <arch/interrupt.h>
#include <arch/x86.h>
#include <kernel/thread.h>
#include <kernel/timer.h>


timer_t test_timer;

enum handler_return test_timer_run(timer_t *timer, time_t now, void *arg)
{
    static uint64_t tick = 0;
    printf("test timer tick %d, now 0x%llx\r\n", tick++, now);

    return INT_NO_RESCHEDULE;
}

int start_kernel()
{
    console_init();

    printf("*** welcom chinix ***\r\n");

    thread_init_early();

    platform_init_interrupts();

    platform_init_timer();

    timer_init();

    timer_set_oneshot(&test_timer, 5000, test_timer_run, NULL);
    thread_becomd_idle();

    return 0;
}
