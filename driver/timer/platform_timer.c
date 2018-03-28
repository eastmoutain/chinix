#include <platfomr_timer.h>
#include <errno.h>
#include <sys/types.h>
#include <arch/spinlock.h>
#include <arch/x86.h>


static platform_timer_callback t_callback;
static void* callback_arg;
static spinlock_t lock;


static uint64_t next_trigger_time;
static uint64_t next_trigger_delta;
static uint64_t ticks_per_ms;

static uint64_t timer_delat_timer;
static volatile uint64_t timer_current_time;

static uint64_t divisor;

#define INTERNAL_FREQ   1193182ULL
#define INTERNAL_FREQ_3X 35795464ULL


// Maximum amount of time that can be program on the timer to scheduler the next interrupt, in milliseconds

#define MAX_TIMER_INTERVAL 55

int platform_set_periodic_timer(platform_timer_callback, void *arg, time_t interval)
{
    t_callback = callback;
    callback_arg = arg;

    next_trigger_delta = (uint64_t) interval << 32;
    next_trigger_time = timer_current_time + next_trigger_delta;

    return NO_ERR;
}
