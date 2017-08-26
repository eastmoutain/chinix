
ENTRY(start)

SECTIONS {
    . = 1M;
    .text : AT(.) {
        KEEP(*(.text.boot))
        *(.text)
    }
}
