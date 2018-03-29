#include <platform_timer.h>
#include <errno.h>
#include <sys/types.h>
#include <arch/spinlock.h>
#include <arch/x86.h>
#include <arch/ioport.h>
#include <arch/interrupt.h>
#include <stdio.h>

static platform_timer_callback t_callback;
static void* callback_arg;
static spinlock_t lock;


static uint64_t next_trigger_time;
static uint64_t next_trigger_delta;
static uint64_t ticks_per_ms;

static uint64_t timer_delta_time;
static volatile uint64_t timer_current_time;

static uint16_t divisor;

#define INTERNAL_FREQ   1193182ULL
#define INTERNAL_FREQ_3X 35795464ULL


// Maximum amount of time that can be program on the timer to scheduler the next interrupt, in milliseconds

#define MAX_TIMER_INTERVAL 55

int platform_set_periodic_timer(platform_timer_callback callback, void *arg, time_t interval)
{
    t_callback = callback;
    callback_arg = arg;

    next_trigger_delta = (uint64_t) interval << 32;
    next_trigger_time = timer_current_time + next_trigger_delta;

    return NO_ERR;
}

time_t current_time(void)
{
    time_t time;
    time = (time_t)(timer_current_time >> 32);

    return time;
}

static enum handler_return os_timer_tick(void *arg)
{
    uint64_t delta;
    static uint32_t tick_cnt = 0;
   // if ((tick_cnt & 0xff) == 0)
        printf("timer tick cnt %d\r\n", tick_cnt);
    tick_cnt++;

    timer_current_time += timer_delta_time;
    time_t time = current_time();

    if (t_callback && timer_current_time >= next_trigger_time) {
        delta = timer_current_time - next_trigger_time;
        next_trigger_time = timer_current_time + next_trigger_time - delta;
        return t_callback(callback_arg, time);
    } else {
        return INT_NO_RESCHEDULE;
   }
}

static void set_pit_frequency(uint32_t frequency)
{
    uint32_t count, remainder;

    if (frequency <= 18) {
        count = 0xffff;
    } else if (frequency >= INTERNAL_FREQ) {
        count = 1;
    } else {
        count = INTERNAL_FREQ_3X / frequency;
        remainder = INTERNAL_FREQ_3X % frequency;

        if (remainder >= INTERNAL_FREQ_3X / 2) {
            count += 1;
        }

        count /=3;
        remainder = count % 3;

        if (remainder >= 1) {
            count += 1;
        }
    }

    divisor = count & 0xffff;

    timer_delta_time = (3685982306ULL * count) >> 10;

    outb(I8253_CONTROL_PORT, 0x34);
    outb(I8253_DATA_PORT, divisor & 0xff); // LSB
    outb(I8253_DATA_PORT, divisor >> 8); // MSB
}

void platform_init_timer(void)
{
    timer_current_time = 0;
    ticks_per_ms = INTERNAL_FREQ / 100;
    set_pit_frequency(1000); // 1ms granularity
    register_int_handler(INT_PIT, os_timer_tick, NULL);
    unmask_interrupt(INT_PIT);
}
