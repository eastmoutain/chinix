

ifeq ($(V),)
	V = 
else
	V := @
endif

include mk/toolchain.mk

TOPDIR = $(shell pwd)
BOOTDIR = $(TOPDIR)/kernel/boot

LINKER_LD := $(TOPDIR)/kernel/boot/linker.ld
GRUB_CFG := $(TOPDIR)/tools/grub.cfg

COMM_COMPILEFLAGS := -fasynchromous-unwind-tables -gdwar-2 -fno-pic
COMM_COMPILEFLAGS += -fno-stack-protector -mcmodel=kernel -mno-red-zene
COMM_COMPILEFLAGS += -MT -MP -MD

LDFLAGS := -z max-page-size=4096

CFLAGS += -I./kernel/include/

KERNEL_SRCS :=  kernel/boot/start.S \
			    kernel/boot/gdt.S \
				kernel/main.c \
				kernel/arch.c \
				kernel/arch/mmu.c \

MM_SRCS:=

LIB_SRCS :=


SRCS := $(KERNEL_SRCS) $(MM_SRCS) $(LIB_SRCS)

OBJS = $(patsubst %.c, %.o, $(SRCS:.S=.o))
DEPS = $(OBJS:.o=.d)
#$(info obj is $(OBJS))

ELF := $(BOOTDIR)/x86_os.elf
ISO := $(BOOTDIR)/x86_os.iso

all: $(ELF) $(ISO)

$(ISO):$(ELF)
	$(V) mkdir -p isofile/boot/grub
	$(V) cp $(GRUB_CFG) isofile/boot/grub
	$(V) cp $(ELF) isofile/boot
	$(V) grub-mkrescue -o $(ISO) isofile  2> /dev/null
	$(V) -rm -rf isofile

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

