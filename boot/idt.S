#include <arch/kernel_cfg.h>
#include <arch/descriptor.h>

.text

_isr:
.set i, 0
.rept NUM_INT

.set isr_start, .

.if i == 8 || (i >= 10 && i <= 14) || i == 17
    nop
    nop
    pushq $i
    jmp _common
.else
    pushq $0
    pushq $i
    jmp _common
.endif

.set isr_len, . - isr_start

.set i, i+1
.endr

.fill 256

_common:
    /* save general purpose registers */
    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12
    pushq %r11
    pushq %r10
    pushq %r9
    pushq %r8
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %rbx
    pushq %rbp
    pushq %rsi
    pushq %rdi

    movq %rsp, %rdi     /* pass the  iframe using rdi */

    //call exception_handler

    /* restore general purpose registers */
    popq %rdi
    popq %rsi
    popq %rbp
    popq %rbx
    popq %rdx
    popq %rcx
    popq %rax
    popq %r8
    popq %r9
    popq %r10
    popq %r11
    popq %r12
    popq %r13
    popq %r14
    popq %r15

    addq $16, %rsp
    iretq


.global setup_idt
.type setup_idt, @function
setup_idt:
    mov $_isr, %rsi
    mov $_idt, %rdi
    movl $NUM_INT, %ecx
l1:
    mov %rsi, %rbx
    movw %bx, (%rdi)
    shr $16, %rbx
    movw %bx, 6(%rdi)
    shr $16, %rbx
    movl %ebx, 8(%rdi)

    add $isr_len, %rsi
    add $16, %rdi
    loop l1
    
    lidt _idtr
    ret

.data
.align 8
.type _idtr, @object
_idtr:
    .short _idt_end - _idt - 1 // length
    .quad _idt // pointer
.fill  8

.align 8
.type _idt, @object
_idt:
.set i, 0
.rept NUM_INT-1
    .short 0
    .short CODE_64_SELECTOR // use code selector
    .byte 0
    .byte 0x8e // present ring0 64-bit gate
    .short 0 // isr offset high:16
    .short 0 // isr offset
    .short 0 // isr offset
    .short 0 // 32bit reserved
    .short 0 // 32bit reserved

.set i, i+1
.endr

.global _idt_end
_idt_end:

