#include <platform_timer.h>
#include <errno.h>
#include <sys/types.h>
#include <arch/spinlock.h>
#include <arch/x86.h>
#include <arch/ioport.h>
#include <arch/interrupt.h>
#include <stdio.h>



static const int uart_baud_rate = 115200;
static const int uart_io_port = 0x3f8;


void platform_init_uart(void)
{
    int divisor = 115200 / uart_baud_rate;

    // set up the load divisor latch
    outb(uart_io_port + 3, 0x80);
    // lsb
    outb(uart_io_port + 0, divisor & 0xff);
    // msb
    outb(uart_io_port + 1, divisor >> 8);
    // 8n1
    outb(uart_io_port + 3, 3);
    // enable fifo, clear, 14-byte threadhold
    outb(uart_io_port + 2, 0x07);
}


void uart_putc(char c)
{
    while ((inb(uart_io_port + 5) & (1 << 6)) == 0);

    outb(uart_io_port, c);
}
