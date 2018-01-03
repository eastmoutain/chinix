#include <arch/kernel_cfg.h>
#include <arch/descriptor.h>
#include <arch/mmu.h>

/* The magic number for the Multiboot header. */
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002

#define MULTIBOOT_HEADER_FLAGS 0x00010002

/* The magic number passed by a Multiboot-compliant boot loader. */
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

#define MSR_EFER 0xc0000080
#define EFER_LME 0x00000100

.section ".text.boot"
.code32
.global _start
_start:
    jmp real_start

.align 8

.type multiboot_header, @object
multiboot_header:
    /* magic */
    .int MULTIBOOT_HEADER_MAGIC
    /* flags */
    .int MULTIBOOT_HEADER_FLAGS
    /* checksum */
    .int -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

    /* header_addr */
    .int PHYS(multiboot_header)
    /* load_addr */
    .int PHYS(_start)
    /* load_end_addr */
    .int PHYS(__data_end)
    /* bss_end_addr */
    .int PHYS(__bss_end)
    /* entry_addr */
    .int PHYS(real_start)

real_start:
    cmpl $MULTIBOOT_BOOTLOADER_MAGIC, %eax
    jne 0f
    movl %ebx, PHYS(multiboot_info)

0:
    /* load our new gdt by physical pointer */
    lgdt PHYS(_gdtr_phys)

    /* load our data selectors */
    movw $DATA_SELECTOR, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    /* load initial stack pointer */
    movl $PHYS(_kstack + BOOT_KSTACK_SIZE), %esp

    /* We need to jump to our sane 32 bit CS */
    pushl $CODE_SELECTOR
    pushl $PHYS(.Lfarjump)
    retf

.Lfarjump:

    /* zero the bss section */
bss_setup:
    movl $PHYS(__bss_start), %edi /* starting address of the bss */
    movl $PHYS(__bss_end), %ecx   /* find the length of the bss in bytes */
    subl %edi, %ecx
    shrl $2, %ecx       /* convert to 32 bit words, since the bss is aligned anyway */
2:
    movl $0, (%edi)
    addl $4, %edi
    loop 2b

paging_setup:
    /* Preparing 64 bit paging, we will use 2MB pages covering 1GB
    for initial bootstrap, this page table will be 1 to 1  */

    /* PAE bit must be enabled  for 64 bit paging*/
    mov %cr4, %eax
    btsl $(5), %eax
    mov %eax, %cr4

    /* load the physical pointer to the top level page table */
    movl $PHYS(pml4), %eax
    mov %eax, %cr3

    /* Long Mode Enabled at this point*/
    movl $MSR_EFER ,%ecx
    rdmsr
    orl $EFER_LME,%eax
    wrmsr

    /* Setting the First PML4E with a PDP table reference*/
    movl $PHYS(pdp), %eax
    orl  $X86_KERNEL_PD_FLAGS, %eax
    movl %eax, PHYS(pml4)

    /* Setting the First PDPTE with a Page table reference*/
    movl $PHYS(pte), %eax
    orl  $X86_KERNEL_PD_FLAGS, %eax
    movl %eax, PHYS(pdp)

    /* point the pml4e at the second high PDP (for -2GB mapping) */
    movl $PHYS(pdp_high),   %eax
    orl  $X86_KERNEL_PD_FLAGS, %eax
    movl %eax, PHYS(pml4 + 8*511)

    /* point the second pdp at the same low level page table */
    movl $PHYS(pte), %eax
    orl  $X86_KERNEL_PD_FLAGS, %eax
    movl %eax, PHYS(pdp_high + 8*510)

    /* map the first 1GB in this table */
    movl $PHYS(pte), %esi
    movl $0x200, %ecx
    xor  %eax, %eax

0:
    mov  %eax, %ebx
    shll $21, %ebx
    orl  $X86_KERNEL_PD_LP_FLAGS, %ebx
    movl %ebx, (%esi)
    addl $8,%esi
    inc  %eax
    loop 0b

    /* set up a linear map of the first 64GB at 0xffffff8000000000 */
    movl $PHYS(linear_map_pdp), %esi
    movl $32768, %ecx
    xor  %eax, %eax

    /* loop across these page tables, incrementing the address by 2MB */
0:
    mov  %eax, %ebx
    shll $21, %ebx
    orl  $X86_KERNEL_PD_LP_FLAGS, %ebx    # lower word of the entry
    movl %ebx, (%esi)
    mov  %eax, %ebx
    shrl $11, %ebx      # upper word of the entry
    movl %ebx, 4(%esi)
    addl $8,%esi
    inc  %eax
    loop 0b

    /* point the high pdp at our linear mapping page tables */
    movl $PHYS(pdp_high), %esi
    movl $64, %ecx
    movl $PHYS(linear_map_pdp), %eax
    orl  $X86_KERNEL_PD_FLAGS, %eax

0:
    movl %eax, (%esi)
    add  $8, %esi
    addl $4096, %eax
    loop 0b

    /* Enabling Paging and from this point we are in
    32 bit compatibility mode*/
    mov %cr0,  %eax
    btsl $(31), %eax
    mov %eax,  %cr0

    /* Using another long jump to be on 64 bit mode
    after this we will be on real 64 bit mode */
    pushl $CODE_64_SELECTOR     /*Need to put it in a the right CS*/
    pushl $PHYS(farjump64)
    retf

.align 8
.code64
farjump64:
    /* branch to our high address */
    mov  $highaddr, %rax
    jmp  *%rax

highaddr:
    /* load the high kernel stack */
    mov  $(_kstack + 4096), %rsp

    /* reload the gdtr */
    lgdt _gdtr


    /* set up the idt */
    call setup_idt

    call main
    hlt

0:                          /* just sit around waiting for interrupts */
    hlt                     /* interrupts will unhalt the processor */
    pause
    jmp 0b                  /* so jump back to halt to conserve power */
