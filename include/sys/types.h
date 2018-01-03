#ifndef TYPES_H
#define TYPES_H

#include <limits.h>
#include <stdint.h>
#include <stddef.h>

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef int ssize_t;
#endif

typedef long long off_t;

typedef uintptr_t addr_t;
typedef uintptr_t vaddr_t;
typedef uintptr_t paddr_t;


#endif // TYPES_H

