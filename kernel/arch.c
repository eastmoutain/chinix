

/* erarly stack */
unsigned char _kstack[4096] __attribute((aligned(8)));

__attribute((section(".data"))) void *multiboot_info;

extern unsigned int __bss_start, __bss_end;

