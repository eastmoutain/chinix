

ifeq ($(V),)
	Q = @
else
	Q :=
endif
export Q

PROJNAME := chinix

ROOTDIR = $(shell pwd)
export ROOTDIR

SCRIPTSDIR = $(ROOTDIR)/scripts
export SCRIPTSDIR

KCONFIGDIR = $(ROOTDIR)/kconfig
KCONFIGFILE = $(ROOTDIR)/Kconfig

# include  tool chain
include $(SCRIPTSDIR)/toolchain.mk

CFLAGS += -include $(ROOTDIR)/include/generated/autoconf.h

ARFLAGS +=

LDFLAGS +=


LINKER_LD := $(ROOTDIR)/boot/linker.ld
GRUB_CFG := $(ROOTDIR)/tools/grub.cfg

LIBDIR := arch \
		  driver \
		  init \
		  kernel \
		  libc

BOOTDIR := boot

LIBS = $(addprefix $(ROOTDIR)/,$(foreach dir, $(LIBDIR), $(dir)/lib$(dir).a))
export LIBS

LDDIR += $(addprefix -L$(ROOTDIR)/,$(LIBDIR))
LDLIB += $(addprefix -l,$(LIBDIR))
export LDDIR
export LDLIB

export CFLAGS
export ARFLAGS
export LDFLAGS

ELF=$(ROOTDIR)/$(PROJNAME).elf
ISO=$(ROOTDIR)/$(PROJNAME).iso

all: $(ELF) $(ISO)


lib: $(LIBDIR)
	$(Q) $(foreach dir, $(LIBDIR), \
		$(MAKE) -C $(dir) obj;\
		$(MAKE) -C $(dir) lib libname=lib$(dir).a;)

$(ELF): lib
	$(Q) $(MAKE) -C $(BOOTDIR) binary elf=$@ linker_file=$(LINKER_LD)

$(ISO):$(ELF)
	$(Q) mkdir -v -p isofile/boot/grub
	$(Q) cp -v $(GRUB_CFG) isofile/boot/grub
	$(Q) cp -v $(ELF) isofile/boot
	$(Q) grub-mkrescue -o $(ISO) isofile  2> /dev/null
	$(Q) rm -rf isofile


.PHONY: menuconfig distclean silentoldconfig clean launch_qemu

menuconfig: $(KCONFIGDIR)/mconf $(KCONFIGDIR)/conf
	$(Q) $< -s $(KCONFIGFILE)
	$(Q) $(MAKE) silentoldconfig

$(KCONFIGDIR)/mconf:
	$(Q) $(MAKE) -C $(KCONFIGDIR)

silentoldconfig: $(KCONFIGDIR)/conf
	$(Q) mkdir -p include/generated include/config
	$(Q) $< -s --silentoldconfig $(KCONFIGFILE)

clean:
	$(Q) $(foreach dir, $(BOOTDIR) $(LIBDIR), $(MAKE) -C $(dir) clean;)
	$(Q) -rm -f $(ELF) $(ISO)

distclean: clean
	$(Q) $(MAKE) -C $(KCONFIGDIR) clean
	$(Q) rm -rf include/generated include/config .config

launch_qemu: $(ISO)
	#$(Q) $(QEMU) -cdrom $(ISO) -enable-kvm
	$(Q) $(QEMU) -cdrom $(ISO) -nographic #-enable-kvm

