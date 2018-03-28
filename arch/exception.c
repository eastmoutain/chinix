

#include <arch/x86.h>
#include <sys/types.h>
#include <stdio.h>

/* exceptions */
#define INT_DIVIDE_0        0x00
#define INT_DEBUG_EX        0x01
#define INT_INVALID_OP      0x06
#define INT_DEV_NA_EX       0x07
#define INT_STACK_FAULT     0x0c
#define INT_GP_FAULT        0x0d
#define INT_PAGE_FAULT      0x0e
#define INT_MF              0x10
#define INT_XM              0x13


static void dump_fault_frame(x64_iframe_t *frame)
{
#if 0 // ARCH_X86_32
    printf(" CS:     %04x EIP: %08x EFL: %08x CR2: %08x\n",
            frame->cs, frame->ip, frame->flags, x86_get_cr2());
    printf("EAX: %08x ECX: %08x EDX: %08x EBX: %08x\n",
            frame->ax, frame->cx, frame->dx, frame->bx);
    printf("ESP: %08x EBP: %08x ESI: %08x EDI: %08x\n",
            frame->sp, frame->bp, frame->si, frame->di);
    printf(" DS:     %04x  ES:     %04x  FS:   %04x  GS:     %04x\n",
            frame->ds, frame->es, frame->fs, frame->gs);
#elif 1 //ARCH_X86_64
    printf("============ dump exception frame ============\n");
    printf(" CS:  %016llx RIP: %016llx\n"
           " EFL: %016llx CR2: %016llx\n",
            frame->cs, frame->rip, frame->rflag, x64_get_cr2());
    printf(" RAX: %016llx RBX: %016llx\n"
           " RCX: %016llx RDX: %016llx\n",
            frame->rax, frame->rbx, frame->rcx, frame->rdx);
    printf(" RSI: %016llx RDI: %016llx\n"
           " RBP: %016llx RSP: %016llx\n",
            frame->rsi, frame->rdi, frame->rbp, frame->user_sp);
    printf(" R8:  %016llx R9:  %016llx\n"
           " R10: %016llx R11: %016llx\n",
            frame->r8, frame->r9, frame->r10, frame->r11);
    printf(" R12: %016llx R13: %016llx\n"
           " R14: %016llx R15: %016llx\n",
            frame->r12, frame->r13, frame->r14, frame->r15);
    printf("err code:  %016llx\n", frame->err_code);
    printf("vector no: %016llx\n", frame->vector);
#endif

    // dump the bottom of the current stack
    addr_t stack = (addr_t) frame;

    if (stack != 0) {
        printf("bottom of stack at 0x%08x:\n", (unsigned int)stack);
        //hexdump((void *)stack, 512);
    }
}


static void exception_die(x64_iframe_t *frame, const char *msg)
{
    printf(msg);
    dump_fault_frame(frame);

    for (;;) {
        x64_cli();
        x64_hlt();
    }
}


void x64_exception_handler(x64_iframe_t *frame)
{
    unsigned int vector = frame->vector;

    switch(vector) {
        case INT_DIVIDE_0:
            exception_die(frame, "devide zero exception\n");
            break;

        default:
            exception_die(frame, "unknown exception\n");
            break;
    }

}

