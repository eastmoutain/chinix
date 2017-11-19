
CC ?= gcc
AS ?= as
LD ?= ld

CFLAGS := -Wall -Wextra -O2 -g -finline -fno-common -fasynchronous-unwind-tables
CFLAGS += -gdwarf-2 -fno-pic -fno-stack-protector -mcmodel=kernel
CFLAGS += -mno-red-zone -MT -MP -MD -nostdlib

