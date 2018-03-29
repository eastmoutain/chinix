#ifndef PLATFORM_TIMER_H
#define PLATFORM_TIMER_H

#include <sys/types.h>


typedef enum handler_return (*platform_timer_callback)(void *arg, time_t now);

void platform_init_timer(void);

int platform_set_periodic_timer(platform_timer_callback callback, void *arg, time_t interval);

#endif // PLATFORM_TIMER_H
