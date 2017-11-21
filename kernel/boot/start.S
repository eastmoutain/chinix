#include <boot/kernel_cfg.h>
#include <boot/descriptor.h>
#include <arch/mmu.h>

#define MULTIBOOT_MAGIC 0xe85250d6

// TODO this value should be verified.
#define MULTIBOOT_BOOTLOADER_MAGIC 0x00000000

#define MSR_EFER 0xc0000080
#define EFER_LME 0x00000100

.section .text.boot
.type multiboot_header, @object
multiboot_header:
    .4byte MULTIBOOT_MAGIC
    .4byte 0x0000
    .4byte header_end - multiboot_header
    .4byte -(MULTIBOOT_MAGIC+(header_end - multiboot_header))
    
    .2byte 0
    .2byte 0
    .4byte 8
header_end:

.section .text
.code32

.global _start
.type _start, @function
_start:
    cmpl $MULTIBOOT_MAGIC, %eax
    jne 0f
    movl %ebx, PHYS(multiboot_info)
0:
    lgdt PHYS(gdtr_phys)

    movw $KERNEL_DATA32_SELECTOR, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    movl $PHYS(_kstack + 4096), %esp

    pushl $KERNEL_CODE32_SELECTOR
    pushl $PHYS(new_pm)
    retf

new_pm:
zero_bss:
    /* zero bss section */
    movl $PHYS(__bss_start), %edi
    movl $PHYS(__bss_end), %ecx
    subl %edi, %ecx
    shrl $2, %ecx
1:
    movl $0, (%edi)
    addl $4, %edi
    loop 1b

setup_paging:
    /* enable PAE */
    mov %cr4, %eax
    btsl $5, %eax
    mov %eax, %cr4

    /* load CR3 */
    movl $PHYS(pml4), %eax
    mov %eax, %cr3

    /* enable long mode */
    movl $MSR_EFER, %ecx
    rdmsr
    orl $EFER_LME, %eax
    wrmsr

    /* pml4[0] --> pdp */
    movl $PHYS(pdp), %eax
    orl $X86_KERNEL_PD_FLAGS, %eax
    movl %eax, PHYS(pml4)

    /* pdp[0] --> pte */
    movl $PHYS(pte), %eax
    orl $X86_KERNEL_PD_FLAGS, %eax
    movl %eax, PHYS(pdp)

    /* pml4[511] -> pdp_high (for -2GB mapping) */
    movl $PHYS(pdp_high), %eax
    orl $X86_KERNEL_PD_FLAGS, %eax
    movl %eax, PHYS(pml4 + 8*511)

    /* pdp_high[510] -> pte */
    /* mapping 1GB from virt -2GB to phys 0GB */
    movl PHYS(pte), %eax
    orl $X86_KERNEL_PD_FLAGS, %eax
    movl %eax, PHYS(pdp_high + 8*510)

    /* the first 1GB */
    movl $PHYS(pte), %esi
    movl $0x200, %ecx
    xor %eax, %eax

2:
    mov %eax, %ebx
    shll $21, %ebx
    orl $X86_KERNEL_PD_LP_FLAGS, %ebx
    movl %ebx, (%esi)
    addl $8, %esi
    inc %eax
    loop 2b

    /* linear_map_pdp maps 64GB from virt -512GB to phys 0GB */
    movl $PHYS(linear_map_pdp), %esi
    movl $32768, %ecx
    xor %eax, %eax
3:
    mov %eax, %ebx
    shll $21, %ebx
    orl $X86_KERNEL_PD_LP_FLAGS, %ebx
    movl %ebx, (%esi)
    mov %eax, %ebx
    shrl $11, %ebx
    movl %ebx, 4(%esi)
    addl $8, %esi
    inc %eax
    loop 3b

    /* pdp_high[0~63] -> linear_map_pdp[0 + .4K.] */
    movl $PHYS(pdp_high), %esi
    movl $64, %ecx
    movl $PHYS(linear_map_pdp), %eax
    orl $X86_KERNEL_PD_FLAGS, %eax
4:
    movl %eax, (%esi)
    add $8, %esi
    addl $4096, %eax
    loop 4b

    /* enable paging */
    mov %cr0, %eax
    btsl $31, %eax
    mov %eax, %cr0

    /* jump to 64bit long mode */
    pushl $KERNEL_CODE64_SELECTOR
    pushl $PHYS(long_mode)
    retf

.align 8
.code64
long_mode:
    //mov $highaddr, %rax
    //jmp *%rax

highaddr:
    mov $(_kstack + 4096), %rsp

    /* reload gdtr, refer to its virt addr */
    lgdt gdtr

    movl $0x2f4b2f4f, 0xb8000
    hlt

