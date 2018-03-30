#ifndef WAIT_QUEUE_H
#define WAIT_QUEUE_H

#include <sys/types.h>
#include <list.h>
#include <stdbool.h>

struct wait_queue {
    struct list_node list;
    int count;
};

typedef struct wait_queue wait_queue_t;

#define WAIT_QUEUE_INITIAL_VALUE(q) \
{ \
    .list = LIST_INITIAL_VALUE((q).list), \
    .count = 0, \
}


void wait_queue_init(wait_queue_t *q);

void wait_queue_destroy(wait_queue_t *q, bool reschedule);

int wait_queue_block(wait_queue_t *q, time_t timeout);


int wait_queue_wake_one(wait_queue_t *q, bool reschedule, int error);
int wait_queue_wake_all(wait_queue_t *q, bool reschedule, int error);

#endif // WAIT_QUEUE_H

