
ENTRY(start)

SECTIONS {
    . = 1M;
    .text : AT(.) {
        KEEP(*(.text.boot))
        *(.text)
        *(.text.*)
        *(.data)
        *(.data.*)
        *(.rodata)
        *(.rodata*)
        *(.bss)
        *(.bss.*)
    }
}
