#V=1
ifeq ($(V),)
	V =
else
	V := @
endif

PROJNAME := chinix

include mk/toolchain.mk

TOPDIR = $(shell pwd)
BOOTDIR = $(TOPDIR)/boot


LINKER_LD := $(TOPDIR)/boot/linker.ld
GRUB_CFG := $(TOPDIR)/tools/grub.cfg

CFLAGS := -Wall -Wextra -O2 -g -finline -fno-common -fasynchronous-unwind-tables \
          -gdwarf-2 -fno-pic -fno-stack-protector -mcmodel=kernel \
		  -mno-red-zone -MT -MP -MD -nostdlib \

CFLAGS += -I./include/ -I./include/driver/ -I./libc/include/

LDFLAGS := -z max-page-size=4096

BOOT_SRC := boot/start.S \
                   boot/gdt.S \
	               boot/idt.S \

ARCH_SRC := arch/arch.c \
                   arch/mmu.c \
				   arch/exception.c \

DRIVER_SRC := driver/console/console.c \
                     driver/interrupt/interrupt.c \
					 driver/timer/platform_timer.c \
					 driver/uart/uart.c \

KERNEL_SRC := kernel/timer.c \

INIT_SRC := init/main.c \

LIBC_SRC := libc/stdio.c \
	        libc/string.c \
	        libc/printf.c \
			libc/debug.c \

KERNEL_SRCS := $(BOOT_SRC) \
	           $(ARCH_SRC) \
			   $(DRIVER_SRC) \
			   $(KERNEL_SRC) \
			   $(INIT_SRC) \
			   $(LIBC_SRC)

SRCS := $(KERNEL_SRCS)

OBJS = $(patsubst %.c, %.o, $(SRCS:.S=.o))
DEPS = $(OBJS:.o=.d)
#$(info obj is $(OBJS))

ELF := $(BOOTDIR)/$(PROJNAME).elf
ISO := $(BOOTDIR)/$(PROJNAME).iso

all: $(ELF) $(ISO)

$(ISO):$(ELF)
	$(V) mkdir -p isofile/boot/grub
	$(V) cp $(GRUB_CFG) isofile/boot/grub
	$(V) cp $(ELF) isofile/boot
	$(V) grub-mkrescue -o $(ISO) isofile  2> /dev/null
	$(V) rm -rf isofile

$(ELF):$(OBJS)
	$(V) echo "ld $@"
	$(V) $(LD) -n $(LDFLAGS) -dT $(LINKER_LD) $^ -o $@

%.o:%.S
	$(V) echo "cc -c $< -o $@"
	$(V) $(CC) $(CFLAGS) -c $< -o $@

%.o:%.c
	$(V) echo "cc -c $< -o $@"
	$(V) $(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean launch_qemu

clean:
	$(V) rm $(OBJS) $(DEPS) $(ELF) $(ISO)

launch_qemu: $(ISO)
	$(V) qemu-system-x86_64 -cdrom $(ISO) -enable-kvm

