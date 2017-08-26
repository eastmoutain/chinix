# x86_64-os

## Description


---

## build
  run "make" to build ELF file.
  
  run "make iso" to generate ISO file, which can be boot from qemu

---

## run
#### run from qemu
  qemu-system-x86-64 -cdrom start.iso

  This command will bring up qemu and boot start.iso, you will see green text "OK" on the screen.
