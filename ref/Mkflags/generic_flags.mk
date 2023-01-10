ARCH?=aarch64
PROTOCOL?=lbp
PLAT?=virt
CROSS_COMPILE?=aarch64-linux-gnu-
KERNEL?=thirdparty/Image

CFLAGS:=-Wall -Wextra -Werror -ffreestanding -fno-stack-protector -fno-zero-initialized-in-bss -Ofast -g -c
LDFLAGS:=-nostdlib -T link.ld

CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
AS=$(CROSS_COMPILE)as
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump
