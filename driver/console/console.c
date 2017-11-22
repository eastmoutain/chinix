#include <console.h>

#include <arch/kernel_cfg.h>

#define FRAME_BUFFER_BASE_ADDR (0xb8000UL)
#define FRAME_BUFFER_VIRT_ADDR (FRAME_BUFFER_BASE_ADDR + KERNEL_SPACE_BASE)

#define FRAME_HEIGHT    (80)
#define FRAME_WIDTH     (25)


