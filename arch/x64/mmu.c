#include <sys/types.h>
#include <compiler.h>
#include <arch/mmu.h>
#include <arch/map.h>

paddr_t pml4[NO_OF_PT_ENTRIES] __ALIGNED(PAGE_SIZE);
paddr_t pdp[NO_OF_PT_ENTRIES] __ALIGNED(PAGE_SIZE);
paddr_t pdp_high[NO_OF_PT_ENTRIES] __ALIGNED(PAGE_SIZE);
paddr_t pte[NO_OF_PT_ENTRIES] __ALIGNED(PAGE_SIZE);
paddr_t linear_map_pdp[(64ULL << 30) / (2ULL << 20)] __ALIGNED(PAGE_SIZE);
