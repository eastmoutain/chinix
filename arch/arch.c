
#include <boot/kernel_cfg.h>

/* erarly stack */
unsigned char _kstack[BOOT_KSTACK_SIZE] __attribute__((aligned(8)));

void *multiboot_info __attribute__((section(".data"))) ;

extern unsigned int __bss_start, __bss_end;

