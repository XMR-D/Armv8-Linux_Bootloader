include Mkflags/generic_flags.mk

srcs-y:=entry.S C-src/kmain.c C-src/serial.c C-src/commands.c C-src/tools.c C-src/CRC32.c C-src/Open-src/bn.c C-src/Open-src/RSA_Montgomery.c C-src/Open-src/sha256.c C-src/rsa.c C-src/Open-src/aes.c

# Convert .c and .S files to .o files in objs
objs:=$(patsubst %.c,%.c.o,$(srcs-y))
objs:=$(patsubst %.S,%.S.o,$(objs))

.PHONY: all
all: build

run: build
	qemu-system-aarch64 -machine virt -cpu cortex-a72 -nographic -kernel pflash.bin -initrd thirdparty/initramfs.cpio.gz -serial mon:stdio -m 2G -smp 4

screen : build
	qemu-system-aarch64 -machine virt -cpu cortex-a72 -nographic -kernel pflash.bin -initrd thirdparty/initramfs.cpio.gz -serial pty -m 2G -smp 4


build: my_bootloader.img my_bootloader.dump pflash.bin
	python3 Py-src/Remap.py
	python3 Py-src/Magic.py
	python3 Py-src/Encrypt.py
	$(info BUILD COMPLETED)



%.c.o: %.c
	$(V)$(CC) $(CFLAGS) $^ -o $@

%.S.o: %.S
	$(V)$(AS) $(SFLAGS) $^ -o $@

%.ld: %.lds
	$(V)$(AS) $^ -o $@

.PRECIOUS: %.elf
%.elf: $(objs)
	$(V)$(LD) $(LDFLAGS) $^ -o $@

.PRECIOUS: %.dump
%.dump: %.elf
	python3 Py-src/Header.py
	$(V)$(OBJDUMP) -D $^ > $@

.PRECIOUS: %.img
%.img: %.elf
	$(OBJCOPY) -O binary $< $@

.PRECIOUS: pflash.bin
pflash.bin: my_bootloader.img
	$(V)dd if=/dev/zero of=$@ bs=1M count=512
	$(V)dd if=$< of=$@ conv=notrunc bs=1M count=20
	$(V)dd if=$(KERNEL) of=$@ conv=notrunc bs=1M seek=50


.PHONY: clean
clean:
	rm -rf *.elf *.bin *.o */*.o */*/*.o *.img *.dump *.key
