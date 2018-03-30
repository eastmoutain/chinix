#include <sys/types.h>
#include <kernel/wait_queue.h>
#include <kernel/thread.h>
#include <kernel/timer.h>
#include <errno.h>
#include <list.h>
#include <stdbool.h>
#include <assert.h>

void wait_queue_init(wait_queue_t *q)
{
    *q = (wait_queue_t)WAIT_QUEUE_INITIAL_VALUE(*q);
}

void wait_queue_destroy(wait_queue_t *q, bool reschedule)
{
    debug_assert(arch_ints_disabled());

    wait_queue_wake_all(q, reschedule, ERR_OBJECT_DESTORYED);
}

static enum handler_return wait_queue_timeout_handler(timer_t *timer, time_t now, void *arg)
{
    thread_t *thread = (thread_t *)arg;

    // FIXME: timer ISR is called in interrupt, in one core platform,
    // it's safe here.

    enum handler_return ret = INT_NO_RESCHEDULE;

    if (thread_unblock_from_wait_queue(thread, ERR_TIMED_OUT) >= NO_ERR) {
        ret = INT_RESCHEDULE;
    }

    return ret;
}

int wait_queue_block(wait_queue_t *q, time_t timeout)
{
    timer_t timer;

    thread_t *thread = get_current_thread();

    debug_assert(thread->state == THREAD_RUNNING);
    debug_assert(arch_ints_disabled());

    if (timeout == 0)
        return ERR_TIMED_OUT;

    list_add_tail(&q->list, &thread->queue_node);
    q->count++;
    thread->state = THREAD_BLOCKED;
    thread->blocking_wait_queue = q;
    // clear return code
    thread->wait_queue_block_ret = NO_ERR;

    if (timeout != INFINIT_TIME) {
        timer_initialize(&timer);
        timer_set_oneshot(&timer, timeout, wait_queue_timeout_handler, (void*)thread);
    }

    thread_resched();

    if (timeout != INFINIT_TIME) {
        timer_cancel(&timer);
    }

    return thread->wait_queue_block_ret;
}


int wait_queue_wake_one(wait_queue_t *q, bool reschedule, int error)
{
    thread_t *blocked_thread;
    struct list_node *node;
    int ret = 0;

    thread_t *thread = get_current_thread();

    debug_assert(arch_ints_disabled());

    node = list_remove_head(&q->list);
    if (node) {
        blocked_thread = container_of(node, thread_t, queue_node);
        q->count--;
        debug_assert(blocked_thread->state == THREAD_BLOCKED);
        blocked_thread->state = THREAD_READY;
        blocked_thread->wait_queue_block_ret = error;
        blocked_thread->blocking_wait_queue = NULL;

        if (reschedule) {
            thread->state = THREAD_READY;
            insert_in_run_queue_head(thread);
        }

        insert_in_run_queue_head(blocked_thread);

        if (reschedule)
            thread_resched();

        ret = 1;
    }

    return ret;
}

int wait_queue_wake_all(wait_queue_t *q, bool reschedule, int error)
{
    thread_t *blocked_thread;
    struct list_node *node;
    int ret = 0;

    thread_t *thread = get_current_thread();

    debug_assert(arch_ints_disabled());

    if (reschedule && q->count > 0) {
        thread->state = THREAD_READY;
        insert_in_run_queue_head(thread);
    }

    for (; ;) {
        node = list_remove_head(&q->list);
        if (node) {
            blocked_thread = container_of(node, thread_t, queue_node);
            q->count--;
            debug_assert(blocked_thread->state == THREAD_BLOCKED);
            blocked_thread->state = THREAD_READY;
            blocked_thread->wait_queue_block_ret = error;
            blocked_thread->blocking_wait_queue = NULL;

            insert_in_run_queue_head(blocked_thread);
            ret++;
        } else
            break;
    }

    if (ret > 0 && reschedule) {
        thread_resched();
    }

    return ret;
}


