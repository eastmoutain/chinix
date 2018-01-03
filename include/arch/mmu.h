#ifndef MMU_H
#define MMU_H

#include <arch/kernel_cfg.h>

#define X86_MMU_PG_P            0x001 // P valid
#define X86_MMU_PG_RW           0x002 // R/W
#define X86_MMU_PG_U            0x004 // U/S user/supervisor
#define X86_MMU_PG_PS           0x080 // PS Page Size
#define X86_MMU_PG_PTE_PAT      0x080 // PAT index
#define X86_MMU_PG_G            0x100 // G Global
#define X86_MMU_CLEAR           0x0
#define X86_DIRTY_ACCESS_MASK   0xf9f
#define X86_MMU_CACHE_DISABLE   0x010 // C Cache disable

#define X86_KERNEL_PD_FLAGS (X86_MMU_PG_RW + X86_MMU_PG_P)
#define X86_KERNEL_PD_LP_FLAGS (X86_MMU_PG_G + X86_MMU_PG_PS + X86_MMU_PG_RW + X86_MMU_PG_P)

#define X86_PDPT_ADDR_MASK  (0x00000000ffffffe0ul)
#define X86_PG_FRAME        (0xfffffffffffff000ul)
#define X86_PHY_ADDR_MASK   (0x000ffffffffffffful)
#define X86_FLAGS_MASK      (0x0000000000000ffful)  /* NX Bit is ignored in the PAE      mode */
#define X86_PTE_NOT_PRESENT (0xFFFFFFFFFFFFFFFEul)
#define X86_2MB_PAGE_FRAME  (0x000fffffffe00000ul)
#define PAGE_OFFSET_MASK_4KB    (0x0000000000000ffful)
#define PAGE_OFFSET_MASK_2MB    (0x00000000001ffffful)
#define X86_MMU_PG_NX       (1ul << 63)

#define X86_PAGING_LEVELS   4
#define PML4_SHIFT          39
#define PDP_SHIFT           30
#define PD_SHIFT            21
#define PT_SHIFT            12
#define PAGE_SHIFT          12
#define ADDR_OFFSET         9
#define PDPT_ADDR_OFFSET    2
#define NO_OF_PT_ENTRIES    512

#define PAGE_SIZE (1<<PAGE_SHIFT)

#define virt2phys(x) ((uintptr_t)(x) - KERNEL_SPACE_BASE)
#define phys2virt(x) ((uintptr_t)(x) + KERNEL_SPACE_BASE)

#endif // MMU_H

