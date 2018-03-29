#include <sys/types.h>
#include <assert.h>
#include <kernel/wait_queue.h>
#include <kernel/thread.h>
#include <kernel/timer.h>
#include <arch/spinlock.h>
#include <stdio.h>

spinlock_t thread_lock = SPINLOCK_INITIAL_VALUE;


static struct list_node thread_list;

stat:ic struct list_node run_queue[NUM_PRIORITY];

static uint32_t run_queue_bitmap;

static thread_t idle_thread;

static void insert_in_run_queue_head(thread_t *thread)
{
    debug_assert(thread->state == THREAD_READY);
    debug_assert(!list_in_list(&thread->queue_node));
    debug_assert(arch_ints_disabled());
    debug_assert(spinlock_held(&thread_lock));

    list_add_head(&run_queue[thread->priority], &thread->queue_node);
    run_queue_bitmap |= 1<<thread->priority;
}

static void insert_in_run_queue_tail(thread_t *thread)
{
    debug_assert(thread->state == THREAD_READY);
    debug_assert(!list_in_list(&thread->queue_node));
    debug_assert(arch_ints_disabled());
    debug_assert(spinlock_held(&thread_lock));

    list_add_tail(&run_queue[thread->priority], &thread->queue_node);
    run_queue_bitmap |= 1<<thread->priority;
}

static void init_thread_strcut(thread_t *thread, cosnt char *name)
{
    memset(thread, 0, sizeof(thread_t));

    assert(strlen(name) < TREAD_NAME_LEN);

    strncpy(thread->name, name, strlen(name));
}

static void init_thread_stack(thread_t *thread)
{
    addr_t *stack_top = thread->stack + thread->stack_size;

    stack_top = stack_top & (~0xfULL); // aligned down to 16 bytes;
    stack_top -= 8;

    struct x64_context_frame *frame = (strcut x64_context_frame*)stack_top;

    frame--;
    memset(frame, 0, sizeof(*frame));

    frame->rip = (addr_t)initial_thread_func;
    frame->rflags = 0x3002;

    // FIXME leave FPU

    thread->sp = (addr_t)frame;
}

static thread_t* get_new_thread(void)
{
    thread_t *new;

    uint32_t bitmap = run_queue_bitmap;
    int q_up_bound = sizeof(run_queue_bitmap) * 8;
    for (int q = 0; q < q_up_bound; q++) {
        if (bitmap & 0x1)
            break;
    }

    if (q >= q_up_bound) {
        goto idle;
    }

    debug_assert(!list_is_empty(&run_queue[q]));

    list_foreach_entry(&run_queue[q], new, thread_t, queue_node) {
        list_delete(new);
        if (list_is_empty(&run_queue[q])) {
            run_queue_bitmap &= ~(1<<q);
        }

        return new;
    }

idle:
    return idle_thread();
}


void thread_become_idle(void);
void thread_set_name(const char *name);
void thread_set_priority(int priority);


thread_t* thread_create(thread_t *thread, const char *name, thread_start_routine entry, void *arg, int priority, void *stack, size_t stack_size)
{
    spinlock_saved_state_t state;

    init_thread_strcut(thread, name);

    thread->entry = entry;
    thread->arg = arg;
    thread->priority = priority;
    thread->state = THREAD_SUSPENDED;
    thread->blocking_wait_queue = NULL;
    thread->wait_queue_block_ret = NO_ERR;

    thread->retcode = 0;
    wait_queue_init(&thread->retcode_wait_queue);

    memset(stack, 0, stack_size);
    thread->stack = stack;
    thread->stack_size = stack_size;

    init_thread_stack(thread);

    THREAD_LOCK(state);
    list_add_tail(&thread_list, &thread->thread_list_node);
    THREAD_UNBLOCK(state);

}

void thread_resched(void)
{
    thread_t *old;
    thread_t *new;

    old = get_current_thread();
    new = get_new_thread();

    debug_assert(new);

    new->state = THREAD_RUNNING;

    if (new == old)
        return;

    if (new->remaining_quantum <= 0)
        new->remaining_quantum = 5;

    set_current_thread(new);

    thread_context_switch(old, new);
}

void thread_yield(thread_t *thread)
{
    spinlock_saved_state_t state;
    thread_t *old = get_current_thread();

    debug_assert(old->state == THREAD_RUNNING);

    THREAD_LOCK(state);

    old->state= THREAD_READY;
    old->remaining_quantum = 0;

    if (!thread_is_idle(thread)) {
        insert_in_run_queue_tail(thread);
    }

    thread_resched();

    THREAD_UNLOCK(state);
}


int thread_resume(thread_t *thread)
{
    spinlock_saved_state_t state;

    debug_assert(thread->state != THREAD_DEATH);
    bool resched = false;
    bool ints_disabled = arch_ints_disabled();

    THREAD_LOCK(state);
    if (thread->state == THREAD_SUSPENDED) {
        thread->state = THREAD_READY;
        insert_in_run_queue_head(thread);
        if (!ints_disabled)
            resched = true;
    }

    THREAD_UNLOCK(state);
    if (resched)
        thread_yield();

    return NO_ERR;
}


int thread_join(thread_t *thread, int *retcode, time_t timeout)
{
    spinlock_saved_state_t state;

    THREAD_LOCK(state);

    if (thread->flags & THREAD_FLAG_DETACHED) {
        THREAD_UNLOCK(state);
        return -ERR_THREAD_DETACHED;
    }

    if (thread->state != THREAD_DEATH) {
        int err = wait_queue_block(&thread->retcode_wait_queue, timeout);
        if (err < 0) {
            THREAD_UNLOCK(state);
            return err;
        }
    }

    debug_assert(thread->state == THREAD_DEATH);
    debug_assert(thread->blocking_wait_queue == NULL);
    debug_assert(!list_in_list(thread->queue_node));

    if (retcode)
        *retcode = thread->retcode;

    list_delete(&thread->thread_list_node);

    THREAD_UNLOCK(state);

    return NO_ERR;
}

int thread_detach(thread_t *thread)
{
    spinlock_saved_state_t state;
    THREAD_LOCK(state);

    wait_queue_wake_all(&thread->retcode_wait_queue, false, ERR_THREAD_DETACHED);

    if (thread->state == THREAD_DEATH) {
        thread->flags &= ~THREAD_FLAG_DETACHED;
        THREAD_UNLOCK(state);
        return thread_join(thread, NULL, 0);
    } else {
        thread->flags |= THREAD_FLAG_DETACHED;
        THREAD_UNLOCK(state);
        return NO_ERR;
    }
}

int thread_detach_and_resume(thread_t *thread);


void thread_exit(int retcode);
void thread_sleep(time_t delay);

void thread_block(void);
void thread_unblock(thread_t *thread, bool resched);

enum hander_return thread_timer_tick(void);

thread_t* get_current_thread(void);
void set_current_thread(thread_t *thread);



