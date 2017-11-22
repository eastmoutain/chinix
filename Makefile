V=1
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
          
CFLAGS += -I./include/

LDFLAGS := -z max-page-size=4096

KERNEL_BOOT_SRC := boot/start.S \
                   boot/gdt.S \
	               boot/idt.S \

KERNEL_ARCH_SRC := arch/arch.c \
                   arch/mmu.c \
			       arch/string.c\

KERNEL_INIT_SRC := init/main.c \

KERNEL_SRCS := $(KERNEL_BOOT_SRC) $(KERNEL_ARCH_SRC) $(KERNEL_INIT_SRC)

MM_SRCS:=

LIB_SRCS :=


SRCS := $(KERNEL_SRCS) $(MM_SRCS) $(LIB_SRCS)

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
	$(V) qemu-system-x86_64 -cdrom $(ISO) --enable-kvm
