#include <kernel/wait_queue.h>
#include <list.h>
#include <stdbool.h>
#include <assert.h>

void wait_queue_init(wait_queue_t *q);

void wait_queue_destroy(wait_queue_t *q, time_t timeout);

int wait_queue_block(wait_queue_t *q, time_t timeout);


int wait_queue_wake_one(wait_queue_t *q, bool reschedule, int error);
int wait_queue_wake_one(wait_queue_t *q, bool reschedule, int error);

int thread_unblock_from_wait_queue(struct thread_t *thread, int error);


