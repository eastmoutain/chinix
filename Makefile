
ifeq ($(V),)
	V = 
else
	V ?= @
endif

CC := x86_64-linux-gnu-gcc
LD := x86_64-linux-gnu-ld

LINKER_LD := linker.ld

CFLAGS := -O2 -g -finline -fno-common -fasynchronous-unwind-tables -gdwarf-2 -fno-pic -fno-stack-protector -mcmodel=kernel -mno-red-zone -MT -MP -MD

start_obj := start.o
ELF := start.elf
ISO := start.iso

all: $(ELF)

$(ELF):$(start_obj)
	$(V) $(LD) -n -T $(LINKER_LD) $^ -o $@

%.o:%.s
	$(V) $(CC) $(CFLAGS) -c $< -o $@ 

.PHONY: clean iso

clean:
	$(V) rm $(start_obj) $(ELF) $(ISO)

iso :$(ELF)
	$(V) mkdir -p isofile/boot/grub
	$(V) cp grub.cfg isofile/boot/grub
	$(V) cp $(ELF) isofile/boot
	$(V) grub-mkrescue -o $(ISO) isofile
	$(V) rm -rf isofile
