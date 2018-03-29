#ifndef X86_H
#define X86_H

#include <compiler.h>
#include <sys/types.h>
#include <stdbool.h>

__BEGIN_CDECLS

/**
 * exception frame on stack
 */
struct x64_eframe {
    // pushed by command "pusha"
    uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    // exception vector
    uint64_t vector;
    // exception error code
    uint64_t err_code;
    // pushed by hardware
    uint64_t rip, cs, rflag;
    // pushed by hardware if privilege should change
    uint64_t user_sp, user_ss;
};

typedef struct x64_eframe x64_iframe_t;

struct x64_context_frame {
    uint64_t r15, r14, r13, r12;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t rflags;
    uint64_t rip;
};


typedef uint64_t x64_flags_t;

static inline void x64_cli(void)
{
    asm volatile ("cli");
}

static inline void x64_sti(void)
{
    asm("sti");
}

static inline void arch_disable_ints(void)
{
    __asm__ volatile("" ::: "memory");
    x64_cli();
}

static inline void arch_enable_ints(void)
{
    x64_sti();
    __asm__ volatile("" ::: "memory");

}

static inline bool arch_ints_disabled(void)
{
    x64_flags_t flags;

    asm ("pushfq;"
        "popq %%rax\n\t"
        : "=a" (flags)
        :: "memory");

    return !(flags & (1 << 9));
}

static inline void x64_hlt(void)
{
    asm ("hlt");
}


static inline uint64_t x64_get_cr2(void)
{
    uint64_t rv;

    asm __volatile__ ("movq %%cr2, %0"
            : "=r" (rv)
            );

    return rv;
}


static inline uint64_t x64_save_flags(void)
{
    uint64_t state;
    __asm__ volatile (
        "pushfq; \n\t"
        "popq %0 \n\t"
        : "=rm" (state)
        :: "memory");

    return state;
}


static inline void x64_restore_flags(uint64_t flags)
{
    __asm__ volatile (
            "pushq %0;"
            "popfq"
            :: "g" (flags)
            : "memory", "cc");
}

// read byte from port
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ __volatile__ ("inb %1, %0\n\t"
                          : "=a"(ret)
                          : "d"(port));
    return ret;
}

// repeat reading bytes from port
static inline void inbrep(uint16_t port, uint8_t *buf, uint32_t n)
{
    __asm__ __volatile__ ("pushal \n\t"
                          "pushfl \n\t"
                          "cli \n\t"
                          "cld \n\t"
                          "rep insb \n\t"
                          "popfl \n\t"
                          "popal"
                          :: "d"(port),
                          "D"(buf),
                          "c"(n));
}

// read word from port
static inline uint16_t inw(uint16_t port)

{
    uint16_t ret;
    __asm__ __volatile__ ("inw %1, %0\n\t"
                          : "=a"(ret)
                          : "d"(port));
    return ret;
}

// read double word from port
static inline uint32_t ind(uint16_t port)
{
    uint32_t ret;
    __asm__ __volatile__ ("inl %1, %0\n\t"
                          : "=a"(ret)
                          : "d"(port));
    return ret;
}

// write byte to port
static inline void outb(uint16_t port, uint8_t data)
{
    __asm__ __volatile__ ("outb %1, %0\n\t"
                          :: "d"(port), "a"(data));
}

// write word to port
static inline void outw(uint16_t port, uint16_t data)
{
    __asm__ __volatile__ ("outw %1, %0\n\t"
                          :: "d"(port), "a"(data));
}

// write double word to port
static inline void outd(uint16_t port, uint32_t data)
{
    __asm__ __volatile__ ("outl %1, %0\n\t"
                          :: "d"(port), "a"(data));
}



#endif // X86_H

