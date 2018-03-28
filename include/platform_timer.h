#ifndef PLATFORM_TIMER_H
#define PLATFORM_TIMER_H

#include <sys/types.h>

typedef enum handler_return (*hardware_timer_callback)(void *arg, time_t now);

int platfomr_set_periodic_timer(hardware_timer_callback callback, void *arg, time_t interval);

#endif // PLATFORM_TIMER_H
