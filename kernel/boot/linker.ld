
mem_base = 0x0;
kernel_base = 0xffffffff80000000;
kernel_load_offset = 0x200000;
kernel_space_base = 0xffffff8000000000;
kernel_space_size = 0x0000008000000000;
user_space_base = 0x0;
user_space_size = 0x0000800000000000;

ENTRY(_start)

SECTIONS {
    . = kernel_base + kernel_load_offset;

    .text : AT(kernel_load_offset) {
        __code_start = .;
        KEEP(*(.text.boot))
        *(.text)
        *(.text.*)
        __code_end = .;
    } =0x9090

    .rodata : ALIGN(4096) {
        __rodata_start = .;
        *(.rodata*)
        __rodata_end = .;
    }
    .data : ALIGN(4096) {
        __data_start = .;
        *(.data .data.*)
        __data_end = .;
    }

    .bss : ALIGN(4096) {
        __bss_start = .;
        *(.bss*)
        . = ALIGN(8);
        __bss_end = .;
    }

    _end = .;
    _end_of_ram = . + 0x400000;
}
