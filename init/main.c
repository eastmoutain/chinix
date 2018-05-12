#include <console.h>
#include <uart.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <platform_timer.h>
#include <arch/interrupt.h>
#include <arch/x86.h>
#include <kernel/thread.h>
#include <kernel/timer.h>

#define APP_THREAD_STACK_SIZE  (4096)
#define NUM_APP_THREADS (2)

thread_t init_thread;
thread_t app_thread[NUM_APP_THREADS];

unsigned int init_thread_stack[CONFIG_DEFAULT_THREAD_STACK_SIZE];
unsigned int app_thread_stack[NUM_APP_THREADS][CONFIG_DEFAULT_THREAD_STACK_SIZE];

static int app_run(void *arg)
{
    static int cnt[NUM_APP_THREADS] = {0};
    while (1) {
        printf("app thread %d running 0x%x \r\n", (int)arg, cnt[(int)arg]++);
        thread_sleep(1000);
        if ((int)arg == 0)
            thread_exit(0);
    }

    return NO_ERR;
}


static int init_run(void *arg)
{
    for (int i = 0; i < NUM_APP_THREADS; i++) {
        int app_id = thread_create("app", app_run, (void*)i, i, app_thread_stack[i], CONFIG_DEFAULT_THREAD_STACK_SIZE);
        thread_detach(app_id);
        thread_resume(app_id);
    }

    return NO_ERR;
}

int start_kernel()
{
    console_init();

    platform_init_uart();

    printf("\n*** welcom chinix ***\r\n\n");
#ifdef CONFIG_VERSION
    printf("    Chinix version %s\n", CONFIG_VERSION);
#endif

    thread_init_early();

    platform_init_interrupts();

    platform_init_timer();

    timer_init();

    int  init_id = thread_create("init_thread", init_run, NULL, 10, init_thread_stack, CONFIG_DEFAULT_THREAD_STACK_SIZE);
    thread_detach(init_id);
    thread_resume(init_id);

    thread_become_idle();

    return 0;
}
