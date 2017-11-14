
V=1
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

LDFLAGS += -z max-page-size=4096

CFLAGS := -O2 -g -finline -fno-common -fasynchronous-unwind-tables -gdwarf-2 -fno-pic -fno-stack-protector -mcmodel=kernel -mno-red-zone -MT -MP -MD

<<<<<<< HEAD
CFLAGS += -Ikernel/include

KERNEL_ASMSRC := kernel/boot/start.s \
				 kernel/boot/gdt.s \

=======
KERNEL_ASMSRC := kernel/boot/start.s
>>>>>>> 74210df2cfe72f826788fc3fddd0ae588b326de3
KERNEL_ASMOBJ = $(patsubst %.s, %.o, $(KERNEL_ASMSRC))

KERNEL_CSRC :=
KERNEL_COBJ = $(patsubst %.c, %.o, $(KERNEL_CSRC))

KERNEL_OBJ = $(KERNEL_ASMOBJ) $(KERNEL_COBJ)

MM_CSRC := 
MM_COBJ = $(patsubst %.c, %.o, $(MM_CSRC))

LIB_CSRC :=
LIB_COBJ = $(patsubst %.c, %.o, $(LIB_CSRC))

OBJ = $(KERNEL_OBJ) $(MM_OBJ) $(LIB_OBJ)

ELF := $(BOOTDIR)/x86_os.elf
ISO := $(BOOTDIR)/x86_os.iso

all: $(ELF) $(ISO)

$(ISO):$(ELF)
	$(V) mkdir -p isofile/boot/grub
	$(V) cp $(GRUB_CFG) isofile/boot/grub
	$(V) cp $(ELF) isofile/boot
	$(V) grub-mkrescue -o $(ISO) isofile  2> /dev/null
	$(V) -rm -rf isofile

$(ELF):$(OBJ)
	$(V) echo "ld $@"
	$(V) $(LD) -n -T $(LINKER_LD) $^ -o $@

%.o:%.s
	$(V) echo "as $< -o $@"
	$(V) $(AS) $< -o $@

%.o:%.c
	$(V) echo "cc -c $< -o $@"
	$(V) $(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean launch_qemu

clean:
	$(V) rm $(OBJ) $(ELF) $(ISO)


launch_qemu: $(ISO) 
	$(V) qemu-system-x86_64 -cdrom $(ISO)
