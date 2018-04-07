#ifndef COMPILER_H
#define COMPILER_H

#if __GNUC__

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#define __UNUSED        __attribute__((__unused__))
#define __PACKED        __attribute__((packed))
#define __ALIGNED(x)     __attribute__((aligned(x)))
#define __SECTION(x)    __attribute__((section(x)))
#define __PURE          __attribute__((pure))
#define __CONST         __attribute__((const))
#define __NO_RETURN     __attribute__((noreturn))
#define __MALLOC        __attribute__((malloc))
#define __WEAK          __attribute__((weak))
#define __GNU_INLINE    __attribute__((gun_inline))
#define __GET_CALLER(x) __builtin_return_address(0)
#define __GET_FRAME(x)  __builtin_frame_address(0)
#define __NAKED         __attribute__((naked))
#define __ISCONSTANT(x) __builtin_constant_p(x)
#define __NO_INLINE(x)  __attribute__((noinline))
#define __CONSTRUCTOR   __attribute__((constructor))
#define __DESTRUCTOR    __attribute__((destructor))
#define __OPTIMIZE(x)   __attribute__((optimize(x)))

#else
#define likely(x)           (x)
#define unlikely(x)         (x)
#define __UNUSED
#define __PACKED
#define __ALIGNE(x)
#define __SECTION(x)
#define __PURE
#define __CONST
#define __NO_RETURN

#endif // __GUNC__

#ifdef __cplusplus
#define __BEGIN_CDECLS extern "C" {
#define __END_CDECLS }
#else
#define __BEGIN_CDECLS
#define __END_CDECLS
#endif // cplusplus

#endif // COMPILER_H
