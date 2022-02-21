IMAGE=mykernel.iso
BINFILE=mykernel.bin
SRC_DIR=src
INC_DIR=include

# Toolchain
CC=gcc #x86_64-pc-linux-gnu-gcc
AS=nasm
LD=ld  #x86_64-elf-ld

# Sources
CSOURCES=$(wildcard $(SRC_DIR)/*.c)
HSOURCES=$(wildcard $(INC_DIR)/*.h)
ASOURCES=$(sort $(wildcard  $(SRC_DIR)/*.s))

# Objects
COBJ=$(CSOURCES:.c=.o)
AOBJ=$(ASOURCES:.s=.o)

# Toolchain flags
CFLAGS=-m32 -Wall -Werror -nostdlib -fno-builtin -fno-stack-protector -std=gnu99 -ffreestanding -c -g -Wno-unused-variable -fPIC 
LDFLAGS=-melf_i386 -z max-page-size=0x1000
ASFLAGS=-felf

# Include directories
CFLAGS+=-I$(INC_DIR)

.PHONY: $(IMAGE) run

all: $(IMAGE)
	@echo "Done !"

$(IMAGE): $(BINFILE)
	cp $(BINFILE) boot/mykernel.bin
	grub-mkrescue -d /usr/lib/grub/i386-pc/ -o $(IMAGE) ./

run:
	qemu-system-x86_64 -boot d -m 2048 -cdrom mykernel.iso

run_curses:
	qemu-system-x86_64 -boot d -m 2048 -cdrom mykernel.iso -curses


$(BINFILE): $(AOBJ) $(COBJ)
	$(LD) $(LDFLAGS) -Tlink.ld $^ -o $@

%.o: %.c $(HSOURCES)
	$(CC) $(CFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f $(COBJ) $(AOBJ) $(BINFILE) boot/$(BINFILE) $(IMAGE)
