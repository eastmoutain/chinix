#include <debug.h>
#include <assert.h>
#include <stdio.h>
#include <kernel/timer.h>
#include <arch/spinlock.h>
#include <list.h>

spinlock_t timer_lock;

struct list_node timer_queue;

static enum handler_return timer_tick(void *arg, time_t now);

void timer_initialize(timer_t *timer)
{
    *timer = (timer_t)TIMER_INITIAL_VALUE(*timer);
}

static void insert_timer_in_queue(timer_t *timer)
{
    timer_t *entry;

    debug_assert(arch_ints_disabled());

    list_foreach_entry(&timer_queue, entry, timer_t, node) {
        if (TIME_GT(entry->sched_time, timer->sched_time)) {
            list_add_before(&entry->node, &timer->node);
            return;
        }
    }

    list_add_tail(&timer_queue, &timer->node);
}

static void timer_set(timer_t *timer, time_t delay, time_t period, timer_callback callback, void *arg)
{
    time_t now;

    debug_assert(!(list_in_list(&timer->node)));

    now = current_time();
    timer->sched_time = now + delay;
    timer->periodic_time = period;
    timer->callback = callback;
    timer->arg = arg;

    spinlock_saved_state_t state;
    spinlock_irq_save(&timer_lock, state);
    insert_timer_in_queue(timer);
    spinlock_irq_restore(&timer_lock, state);

    printf("register timer: %p, period: 0x%llx, is periodic: %s\r\n",
            timer, period, period?"yes":"no");
}


void timer_set_oneshot(timer_t *timer, time_t delay, timer_callback callback, void* arg)
{
    if (delay == 0)
        delay = 1;

    timer_set(timer, delay, 0, callback, arg);
}

void timer_set_periodic(timer_t *timer, time_t period, timer_callback callback, void* arg)
{
    if (period == 0)
        period = 1;

    timer_set(timer, period, period, callback, arg);
}

void timer_cancel(timer_t *timer)
{
    spinlock_saved_state_t state;
    spinlock_irq_save(&timer_lock, state);

    if (list_in_list(&timer->node))
        list_delete(&timer->node);

    timer->periodic_time = 0;
    timer->callback = NULL;
    timer->arg = NULL;

    spinlock_irq_restore(&timer_lock, state);
}

// FIXME not impleted
static enum handler_return timer_tick(void *arg, time_t now)
{
    timer_t *timer;
    enum handler_return ret = INT_NO_RESCHEDULE;

    debug_assert(arch_ints_disabled());

    // FIXME : spinlock here?
    // as we are in interrupt handler, on one core platform is okay not spin lock
    static uint64_t tick_cnt = 0;
    if ((tick_cnt & 0xff) == 0) {
        printf("tick_cnt 0x%llx\r\n", tick_cnt);
    }
    tick_cnt++;

    struct list_node *node;
    for (; ;) {
        node = list_peek_head(&timer_queue);
        if (node) {
            timer = container_of(node, timer_t, node);
        } else
            break;

        if (TIME_LT(now, timer->sched_time))
            break;

        list_delete(&timer->node);

        // FIXME spin unlock? as the node is off list

        bool periodic = timer->periodic_time > 0;
        if (timer->callback(timer, now, timer->arg) == INT_RESCHEDULE)
            ret = INT_RESCHEDULE;

        // insert periodic timer into timer queue again
        if (periodic) {
            if (!list_in_list(&timer->node) && (timer->periodic_time > 0)) {
                printf("reinsett timer %p in to timer queue\r\n", timer);
                timer->sched_time = now + timer->periodic_time;
                insert_timer_in_queue(&timer);
            }
        }
    }

    // FIXME: spin unlock?

    if (thread_timer_tick() == INT_RESCHEDULE)
        ret = INT_RESCHEDULE;

    return ret;
}


void timer_init(void)
{
    printf("init timer\r\n");

    timer_lock = SPINLOCK_INITIAL_VALUE;

    list_initialize(&timer_queue);

    printf("timer frequency 100Hz\r\n");

    platform_set_periodic_timer(timer_tick, NULL, 10); // 10ms
}

