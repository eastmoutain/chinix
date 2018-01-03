# chinix

## Description


---

## pre-install
  install grub2, xorror

## build
  run "make all" to build ELF and ISO file.
  
  run "make chinix.iso" to generate ISO file, which can be boot from qemu

---

## run
#### run from qemu
  run "make launch_qemu"
  or equivalently run command
  "qemu-system-x86-64 -cdrom chinix.iso [-enable-kvm]"

