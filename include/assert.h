#ifndef ASSERT_H
#define ASSERT_H

#include <debug.h>

#ifdef CONFIG_DEBUG_ASSERT
#define debug_assert(x) \
    do { \
        if(!(x)) \
            platform_panic("\npanic...\n" \
                           "asserted failed at (%s:%d): reason: (%s)\n",\
                           __FILE__, __LINE__, #x); \
    } while (0)
#else
#define debug_assert(x)
#endif // CONFIG_DEBUG_ASSERT

#endif
