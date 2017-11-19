# chinix

## Description


---

## pre-install
  install grub2, xorror

## build
  run "make" to build ELF file.
  
  run "make" to generate ISO file, which can be boot from qemu

---

## run
#### run from qemu
  qemu-system-x86-64 -cdrom chinix.iso

  This command will bring up qemu and boot chinix.iso, you will see green text "OK" on the screen.
