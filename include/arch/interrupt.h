
#ifndef INTERRUPT_H
#define INTERRUPT_H


#define INT_MAX_VECTORS 0x31

#define INT_BASE        0x20
#define INT_PIT         0x20
#define INT_KEYBOARD    0x21
#define INT_PIC2        0x22

#define INT_BASE2       0x28
#define INT_CMOSRTC     0x28
#define INT_PS2MOUSE    0x2c
#define INT_IDE0        0x2e
#define INT_IDE1        0x2f

#define INT_APIC_TIMER      0x22

#define PIC1_BASE   0x20
#define PIC2_BASE   0x28

#include <sys/types.h>
#include <arch/x86.h>

void init_interrupts(void);
int mask_interrupt(uint32_t vector);
int unmask_interrupt(uint32_t vector);
typedef enum handler_return (*int_handler)(void *arg);

void register_int_handler(uint32_t vector, int_handler handler, void *arg);

void platform_init_interrupts(void);

enum handler_return platform_irq(x64_iframe_t *frame);

#endif //INTERRUPT_H
