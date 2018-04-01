#include <console.h>
#include <stdio.h>
#include <assert.h>
#include <platform_timer.h>
#include <arch/interrupt.h>
#include <arch/x86.h>
#include <kernel/thread.h>
#include <kernel/timer.h>


timer_t test_timer;

#define INIT_THREAD_STACK_SIZE  (4096)
#define APP_THREAD_STACK_SIZE  (4096)
#define NUM_APP_THREADS (5)

thread_t init_thread;
thread_t app_thread[NUM_APP_THREADS];

unsigned int init_thread_stack[INIT_THREAD_STACK_SIZE];
unsigned int app_thread_stack[NUM_APP_THREADS][APP_THREAD_STACK_SIZE];

static int app_run(void *arg)
{
    static int cnt[NUM_APP_THREADS] = {0};
    while (1) {
        printf("app thread %d running 0x%x \r\n", (int)arg, cnt[(int)arg]++);
        thread_sleep(1000);
    }
}


static int init_run(void *arg)
{
    for (int i = 0; i < NUM_APP_THREADS; i++) {
        thread_t *app = thread_create(&app_thread[i], "app", app_run, (void*)i, 10, app_thread_stack[i], APP_THREAD_STACK_SIZE);
        thread_detach(app);
        thread_resume(app);
    }
}

static enum handler_return test_timer_run(timer_t *timer, time_t now, void *arg)
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

    thread_t *init = thread_create(&init_thread, "init_thread", init_run, NULL, 10, init_thread_stack, INIT_THREAD_STACK_SIZE);
    thread_detach(init);
    thread_resume(init);

    thread_become_idle();

    return 0;
}
