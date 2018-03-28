#ifndef TIMER_H
#define TIMER_H

#include <list.h>
#include <sys/types.h>

void timer_init(void);

struct timer;

typedef enum handler_return (*timer_callback)(struct timer*, time_t now, void *arg);

struct timer {
    struct list_node node;
    time_t sched_time;
    time_t periodic_time;
    timer_callback callback;
    void *arg;
};

typedef struct timer timer_t;

#define TIMER_INITIAL_VALUE (t) \
{ \
    .node = LIST_INITIAL_CLEARED_VALUE, \
    .sched_time = 0, \
    .periodic_time = 0, \
    .callback = NULL, \
    .arg = NULL, \
}

void timer_initialize(timer_t *timer);
void timer_set_periodic(timer_t *timer, time_t period, timer_callback callback, void *arg);

void timer_cancel(timer_t *timer);

#endif // TIMER_H
