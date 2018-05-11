#ifndef THREAD_H
#define THREAD_H

#include <kernel/wait_queue.h>
#include <arch/x86.h>
#include <sys/types.h>
#include <list.h>
#include <arch/spinlock.h>
#include <assert.h>
#include <stdio.h>

enum thread_state {
    THREAD_SUSPENDED = 0,
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED,
    THREAD_SLEEPING,
    THREAD_DEATH,
};

typedef int (*thread_start_routine)(void *arg);

#define THREAD_FLAG_DETACHED        (1<<0)
#define THREAD_FLAG_IDLE            (1<<1)

#define TREAD_NAME_LEN 31

typedef struct thread {
    struct list_node thread_list_node;

    struct list_node queue_node;

    int priority;
    enum thread_state state;
    int remaining_quantum;

    unsigned int flags;
    // where the thread is blocked
    struct wait_queue *blocking_wait_queue;
    // when thread is waken up, it stores ret errno
    int wait_queue_block_ret;

    void *stack;
    size_t stack_size;

    addr_t sp; // current stack pointer

    thread_start_routine entry;
    void *arg;

    // thread exit code
    int retcode;

    // let its parent wait the thread exit
    struct wait_queue exit_wait_queue;

    char name[TREAD_NAME_LEN + 1];
} thread_t;

#define NUM_PRIORITY 32
#define LOWEST_PRIORITY 0
#define HIGHEST_PRIORITY (NUM_PRIORITY - 1)
#define IDLE_PRIORITY LOWEST_PRIORITY
#define DEFAULT_PRIORITY (NUM_PRIORITY >> 1)
#define HIGH_PRIORITY ((NUM_PRIORITY / 4) * 3)


#define DEFAULT_STACK_SIZE (4096)

void thread_init_early(void);
void thread_init(void);
void thread_become_idle(void);
void thread_set_name(const char *name);
void thread_set_priority(int priority);

thread_t* thread_create(thread_t *thread, const char *name, thread_start_routine entry, void *arg, int priority, void *stack, size_t stack_size);

int thread_resume(thread_t *thread);
int thread_detach(thread_t *thread);
int thread_join(thread_t *thread, int *retcode, time_t timeout);
int thread_detach_and_resume(thread_t *thread);

void insert_in_run_queue_head(thread_t *thread);
void insert_in_run_queue_tail(thread_t *thread);

void thread_exit(int retcode);
void thread_sleep(time_t delay);

void thread_block(void);
void thread_unblock(thread_t *thread, bool resched);

enum handler_return thread_timer_tick(void);

thread_t* get_current_thread(void);
void set_current_thread(thread_t *thread);

void thread_early_init(void);
void thread_become_idle(void);

void thread_preempt(void);

void thread_preempt_enable(void);
void thread_preempt_disable(void);
bool thread_preempt_is_enabled(void);

extern spinlock_t thread_lock;

#define THREAD_LOCK(state) spinlock_irq_save(&thread_lock, state)

#define THREAD_UNLOCK(state) spinlock_irq_restore(&thread_lock, state)

#endif // THREAD_H

