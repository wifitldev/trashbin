ASM=nasm
CC=gcc
LD=ld
OBJCOPY=objcopy
CFLAGS=-ffreestanding -fno-stack-protector -fno-pic -m64 -O2 -Wall -Wextra -nostdlib -nostdinc
LDFLAGS=-nostdlib -z max-page-size=0x1000

BUILD=build
BOOT=$(BUILD)/boot
KERNEL=$(BUILD)/kernel
TOOLS=$(BUILD)/tools

STAGE2_SECTORS=40

all: disk.img

$(BUILD):
	mkdir -p $(BUILD) $(BOOT) $(KERNEL) $(TOOLS)

$(BOOT)/stage1.bin: boot/stage1.asm | $(BUILD)
	$(ASM) -f bin -DSTAGE2_SECTORS=$(STAGE2_SECTORS) $< -o $@

$(BOOT)/stage2.bin: boot/stage2.asm | $(BUILD)
	$(ASM) -f bin -DSTAGE2_SECTORS=$(STAGE2_SECTORS) -DKERNEL_LBA=$$(cat $(BUILD)/kernel.lba) -DKERNEL_SECTORS=$$(cat $(BUILD)/kernel.sectors) -DFS_LBA=$$(cat $(BUILD)/fs.lba) -DFS_SECTORS=$$(cat $(BUILD)/fs.sectors) $< -o $@

KERNEL_OBJS=\
	$(KERNEL)/entry.o \
	$(KERNEL)/idt.o \
	$(KERNEL)/isr.o \
	$(KERNEL)/interrupts.o \
	$(KERNEL)/syscall.o \
	$(KERNEL)/context.o \
	$(KERNEL)/kernel.o \
	$(KERNEL)/console.o \
	$(KERNEL)/printf.o \
	$(KERNEL)/string.o \
	$(KERNEL)/memory.o \
	$(KERNEL)/paging.o \
	$(KERNEL)/heap.o \
	$(KERNEL)/task.o \
	$(KERNEL)/timer.o \
	$(KERNEL)/keyboard.o \
	$(KERNEL)/mouse.o \
	$(KERNEL)/ata.o \
	$(KERNEL)/fs.o \
	$(KERNEL)/vfs.o \
	$(KERNEL)/shell.o \
	$(KERNEL)/gui.o \
	$(KERNEL)/explorer.o

$(KERNEL)/%.o: kernel/arch/x86_64/%.asm | $(BUILD)
	$(ASM) -f elf64 $< -o $@

$(KERNEL)/%.o: kernel/%.c | $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -Ikernel/arch/x86_64 -c $< -o $@

$(KERNEL)/%.o: kernel/drivers/%.c | $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -Ikernel/arch/x86_64 -c $< -o $@

$(KERNEL)/%.o: kernel/fs/%.c | $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -Ikernel/arch/x86_64 -c $< -o $@

$(KERNEL)/%.o: kernel/gui/%.c | $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -Ikernel/arch/x86_64 -c $< -o $@

$(KERNEL)/%.o: kernel/lib/%.c | $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -Ikernel/arch/x86_64 -c $< -o $@

$(KERNEL)/%.o: kernel/mm/%.c | $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -Ikernel/arch/x86_64 -c $< -o $@

$(KERNEL)/%.o: kernel/shell/%.c | $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -Ikernel/arch/x86_64 -c $< -o $@

$(KERNEL)/%.o: kernel/sys/%.c | $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -Ikernel/arch/x86_64 -c $< -o $@

$(KERNEL)/kernel.elf: $(KERNEL_OBJS) kernel/arch/x86_64/linker.ld | $(BUILD)
	$(LD) $(LDFLAGS) -T kernel/arch/x86_64/linker.ld -o $@ $(KERNEL_OBJS)

$(KERNEL)/kernel.bin: $(KERNEL)/kernel.elf
	$(OBJCOPY) -O binary $< $@

$(TOOLS)/mkfs: tools/mkfs.c | $(BUILD)
	$(CC) -O2 -Wall -Wextra $< -o $@

fs.img: $(TOOLS)/mkfs fsroot/*
	$(TOOLS)/mkfs fsroot $@

layout: $(KERNEL)/kernel.bin fs.img | $(BUILD)
	@kernel_size=$$(stat -c%s $(KERNEL)/kernel.bin); \
	kernel_sectors=$$((($$kernel_size + 511) / 512)); \
	fs_size=$$(stat -c%s fs.img); \
	fs_sectors=$$((($$fs_size + 511) / 512)); \
	kernel_lba=$$((1 + $(STAGE2_SECTORS))); \
	fs_lba=$$((kernel_lba + kernel_sectors)); \
	echo $$kernel_lba > $(BUILD)/kernel.lba; \
	echo $$kernel_sectors > $(BUILD)/kernel.sectors; \
	echo $$fs_lba > $(BUILD)/fs.lba; \
	echo $$fs_sectors > $(BUILD)/fs.sectors

stage2: layout $(BOOT)/stage2.bin

image: $(BOOT)/stage1.bin stage2 $(KERNEL)/kernel.bin fs.img
	dd if=/dev/zero of=disk.img bs=1M count=16
	dd if=$(BOOT)/stage1.bin of=disk.img conv=notrunc
	dd if=$(BOOT)/stage2.bin of=disk.img bs=512 seek=1 conv=notrunc
	dd if=$(KERNEL)/kernel.bin of=disk.img bs=512 seek=$$(cat $(BUILD)/kernel.lba) conv=notrunc
	dd if=fs.img of=disk.img bs=512 seek=$$(cat $(BUILD)/fs.lba) conv=notrunc

kernel: $(KERNEL)/kernel.bin

disk.img: image

run: disk.img
	qemu-system-x86_64 -drive format=raw,file=disk.img -m 256M -serial stdio

clean:
	rm -rf $(BUILD) *.img disk.img

.PHONY: all clean run image kernel stage2 layout
