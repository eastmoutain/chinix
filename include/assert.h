#ifndef ASSERT_H
#define ASSERT_H

#include <debug.h>

#define debug_assert(x) \
    do { \
        if(!x) \
            platform_panic("\npanic...\n" \
                           "asserted failed at (%s:%d): reason: %s\n", __FILE__, __LINE__, #x); \
    } while (0)

#endif
