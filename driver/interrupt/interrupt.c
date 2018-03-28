#include <sys/types.h>
#include <stdio.h>
#include <arch/x86.h>
#include <arch/spinlock.h>
#include <arch/interrupt.h>
#include <errno.h>
#include <debug.h>
#include <assert.h>

#define PIC1 0x20
#define PIC2 0xA0

#define ICW1 0x11
#define ICW2 0x01


static spinlock_t interrupt_spinlock;
static uint8_t irqMask[2];


struct int_handler_struct {
    int_handler handler;
    void *arg;
};

static struct int_handler_struct int_handler_table[INT_MAX_VECTORS];

static void map_interrupt_controller(uint32_t pic1, uint32_t pic2)
{
    // send ICW1
    outb(PIC1, ICW1);
    outb(PIC2, ICW1);

    // send ICW2
    outb(PIC1 + 1, pic1); // remap
    outb(PIC2 + 1, pic2);

    // send ICW3
    outb(PIC1 + 1, 4); // IRQ2 connect ot slave
    outb(PIC2 + 1, 2);

    // send ICW4
    outb(PIC1 + 1, 5);
    outb(PIC2 + 1, 1);

    // disable all IRQs
    outb(PIC1 + 1, 0xff);
    outb(PIC2 + 1, 0xff);

    irqMask[0] = 0xff;
    irqMask[1] = 0xff;
}


static interrupt_enable(uint32_t vector, bool enable)
{
    if (vector >= PIC1_BASE && vector < PIC1_BASE + 8) {
        vector -= PIC1_BASE;

        uint8_t bit = 1 << vector;

        if (enable && (irqMask[0] & bit)) {
            irqMask[0] = inb(PIC1 + 1);
            irqMask[0] &= ~bit;
            outb(PIC1 + 1, irqMask[0]);
            irqMask[0] = inb(PIC1 + 1);
        } else if (!enable && !(irqMask[0] & bit)) {
            irqMask[0] = inb(PIC1 + 1);
            irqMask[0] = bit;
            outb(PIC1 + 1, irqMask[0]);
            irqMask[0] = inb(PIC1 + 1);
        }
    } else if (vector >= PIC2_BASE && vector < PIC2_BASE + 8) {
        vector -= PIC2_BASE;

        uint8_t bit = 1 << vector;
        if (enable && (irqMask[1] & bit)) {
            irqMask[1] = inb(PIC2 + 1);
            irqMask[1] &= ~bit;
            outb(PIC2 + 1, irqMask[1]);
            irqMask[1] = inb(PIC2 + 1);
        } else if (!enable && !(irqMask[1] & bit)) {
            irqMask[1] = inb(PIC2 + 1);
            irqMask[1] |= bit;
            outb(PIC2 + 1, irqMask[1]);
            irqMask[1] = inb(PIC2 + 1);
        }

        bit = 1 <<(INT_PIC2 - PIC1_BASE);

        if (irqMask[1] != 0xff && (irqMask[0] & bit)) {
            irqMask[0] = inb(PIC1 + 1);
            irqMask[0] &= ~bit;
            outb(PIC1 + 1, irqMask[0]);
            irqMask[0] = inb(PIC1 + 1);
        } else if (irqMask[1] == 0 && !(irqMask[0] & bit)) {
            irqMask[0] = inb(PIC1 + 1);
            irqMask[0] |= bit;
            outb(PIC1 + 1, irqMask[0]);
            irqMask[0] = inb(PIC1 + 1);
        }
    } else {
        printf("invalid PIC interrupt 0x%x\r\n", vector);
    }
}

// issue end of interrupt
static void issue_eoi(uint32_t vector)
{
    if (vector >= PIC1_BASE && vector <= PIC1_BASE + 7) {
        outb(PIC1, 0x20);
    } else if (vector >= PIC2_BASE && vector <= PIC2_BASE + 7) {
        outb(PIC2, 0x20);
        outb(PIC1, 0x20);
    }
}

void init_interrupts(void)
{
    map_interrupt_controller(PIC1_BASE, PIC2_BASE);
}


int mask_interrupt(uint32_t vector)
{
    if (vector >= INT_MAX_VECTORS) {
        printf("vector 0x%x exceeds max interrupt vector 0x%x\r\n",
                vector, INT_MAX_VECTORS);
        return ERR_ARG;
    }

    spinlock_saved_state_t state;
    spinlock_irq_save(&interrupt_spinlock, state);
    interrupt_enable(vector, false);
    spinlock_irq_restore(&interrupt_spinlock, state);

    return NO_ERR;
}


void mask_all_interrupt(void)
{
    irqMask[0] = inb(PIC1 + 1);
    irqMask[1] = inb(PIC2 + 1);

    outb(PIC1 + 1, 0xff);
    outb(PIC2 + 1, 0xff);

    irqMask[0] = inb(PIC1 + 1);
    irqMask[1] = inb(PIC2 + 1);
}

int unmask_interrupt(uint32_t vector)
{
    if (vector >= INT_MAX_VECTORS) {
        printf("vector 0x%x exceeds max interrupt vector 0x%x\r\n",
                vector, INT_MAX_VECTORS);
        return ERR_ARG;
    }

    spinlock_saved_state_t state;
    spinlock_irq_save(&interrupt_spinlock, state);

    interrupt_enable(vector, true);

    spinlock_irq_restore(&interrupt_spinlock, state);

    return NO_ERR;
}


enum handler_return platform_irq(x64_iframe_t *frame)
{
    unsigned int vector = frame->vector;

    debug_assert(vector >= INT_MAX_VECTORS);

    enum handler_return ret = INT_NO_RESCHEDULE;

    if (int_handler_table[vector].handler)
        ret = int_handler_table[vector].handler(int_handler_table[vector].arg);

    issue_eoi(vector);

    return ret;
}


void register_int_handler(uint32_t vector, int_handler handler, void *arg)
{
   debug_assert(vector >= INT_MAX_VECTORS);

   spinlock_saved_state_t state;
   spinlock_irq_save(&interrupt_spinlock, state);

   int_handler_table[vector].arg = arg;
   int_handler_table[vector].handler = handler;

   spinlock_irq_restore(&interrupt_spinlock, state);
}
