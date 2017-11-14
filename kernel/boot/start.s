
.section ".text.boot"
.type multiboot_header, @object
multiboot_header:
header_start:
    .4byte 0xe85250d6 
    .4byte 0x000
    .4byte header_end - header_start
    .4byte 0x100000000 - (0xe85250d6+(header_end - header_start))
    
    .2byte 0
    .2byte 0
    .4byte 8
header_end:

.section ".text"
.code32
.global start
.type start, @function
start:
    movl $0x2f4b2f4f, 0xb8000
    hlt

