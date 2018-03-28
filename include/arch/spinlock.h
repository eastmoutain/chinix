#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <arch/x86.h>
#include <stdbool.h>

#define SPINLOCK_INIT_VAL   (0)

typedef unsigned long spinlock_t;

typedef x64_flags_t spinlock_saved_state_t;
typedef unsigned int spinlock_save_flag_t;

static inline void spin_lock(spinlock_t *lock)
{
    *lock = 1;
}

static inline int spin_trylock(spinlock_t *lock)
{
    return 0;
}

static inline int spin_unlock(spinlock_t *lock)
{
    *lock = 0;
}

static inline spinlock_init(spinlock_t *lock)
{
    *lock = SPINLOCK_INIT_VAL;
}

static inline bool spinlock_held(spinlock_t *lock)
{
    return *lock != 0;
}

static inline arch_interrupt_save(spinlock_saved_state_t *state)
{
    *state = x64_save_flags();
    arch_disable_ints();
}

static inline arch_interrupt_restore(spinlock_saved_state_t state)
{
    x64_restore_flags(state);
}

static inline void spinlock_save(spinlock_t *lock, spinlock_saved_state_t *state)
{
    arch_interrupt_save(state);
    spin_lock(lock);
}

static inline void spinlock_restore(spinlock_t *lock, spinlock_saved_state_t state)
{
    spin_unlock(lock);
    arch_interrupt_restore(state);
}


#define spinlock_irq_save(lock, state) spinlock_save(lock, &(state))

#define spinlock_irq_restore(lock, state) spinlock_restore(lock, state)

#endif //SPINLOCK_H
