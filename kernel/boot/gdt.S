#include <boot/kernel_cfg.h>

.section .rodata

.align 8
.global gdtr_phys
.type gdtr_phys, @object
gdtr_phys:
    .short _gdt_end - _gdt - 1
    .int PHYS(_gdt)

.global gdtr
.type gdtr, @object
gdtr:
    .short _gdt_end - _gdt - 1
    .quad PHYS(_gdt)

.data
.align 8
.type _gdt, @object
_gdt:
    .int 0
    .int 0

.set kernel_code32_sel, . - _gdt
_kernel_code32_gde:
    .short  0xffff   // limit
    .short  0x0000   // base
    .byte   0x00
    .byte   0b10011010  // P(1) DPL(00) S(1) 1 C(0) R(1) A(0)
    .byte   0b11001111 // G(1) D(1) 0 0 limit 19:16
    .byte   0x00

.set kernel_data32_sel, . - _gdt
_kernel_data32_gde:
    .short 0xffff   // limit
    .short 0x0000   // base
    .byte 0x00      // base 23:16
    .byte 0b10010010 // P(1) DPL(00) S(1) 0 E(0) W(1) A(0)
    .byte 0b11001111 // G(1) B(1) 0 0 limit 19:16
    .byte 0x00

.set user_code32_sel, . - _gdt
_user_code32_gde:
    .short 0xffff
    .short 0x0000
    .byte 0x00
    .byte 0b11111010
    .byte 0b11001111
    .byte 0x00

.set user_data32_sel, . - _gdt
_user_data32_gde:
    .short 0xffff
    .short 0x0000
    .byte 0x00
    .byte 0b11110010
    .byte 0b11001111
    .byte 0x00

.set kernel_code64_sel, . - _gdt
_kernel_code64_gde:
    .short 0xffff
    .short 0x0000
    .byte 0x00
    .byte 0b10011010
    .byte 0b10101111
    .byte 0x00

.set _kernel_data64_sel, . - _gdt
_kernel_data64_gde:
    .short 0xffff
    .short 0x0000
    .byte 0x00
    .byte 0b10010010
    .byte 0b11001111
    .byte 0x00

.set user_code64_sel, . - _gdt
_user_code64_gde:
    .short 0xffff
    .short 0x0000
    .byte 0x00
    .byte 0b11111010
    .byte 0b10101111
    .byte 0x00

.set user_data64_sel, . - _gdt
_user_data64_gde:
    .short 0xffff
    .short 0x0000
    .byte 0x00
    .byte 0b11110010
    .byte 0b11001111
    .byte 0x00

.set tss_sel, . - _gdt
_tss_gde:
    .short 0
    .short 0
    .byte 0
    .byte 0x89
    .byte 0x80
    .byte 0
    .quad 0x0000000000000000

_gdt_end:
