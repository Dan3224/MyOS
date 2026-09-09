BUILD_DIR := build/out
ISO_DIR := build/iso
KERNEL := $(BUILD_DIR)/myos.elf
ISO := build/myos-0.1.iso
CFLAGS := -m32 -ffreestanding -fno-pie -fno-stack-protector -Wall -Wextra
LDFLAGS := -m elf_i386 -T kernel/linker.ld
.PHONY: all run check clean
all: $(ISO)
$(BUILD_DIR):
	mkdir -p $@
$(BUILD_DIR)/boot.o: kernel/boot.S | $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@
$(BUILD_DIR)/console.o: kernel/console.c kernel/console.h | $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@
$(BUILD_DIR)/keyboard.o: kernel/keyboard.c kernel/keyboard.h kernel/io.h | $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@
$(BUILD_DIR)/power.o: kernel/power.c kernel/power.h kernel/io.h | $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@
$(BUILD_DIR)/main.o: kernel/main.c kernel/console.h kernel/io.h | $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@
$(KERNEL): $(BUILD_DIR)/boot.o $(BUILD_DIR)/console.o $(BUILD_DIR)/keyboard.o $(BUILD_DIR)/power.o $(BUILD_DIR)/main.o kernel/linker.ld
	ld $(LDFLAGS) -o $@ $(BUILD_DIR)/boot.o $(BUILD_DIR)/console.o $(BUILD_DIR)/keyboard.o $(BUILD_DIR)/power.o $(BUILD_DIR)/main.o
$(ISO): $(KERNEL) build/iso/boot/grub/grub.cfg
	cp $(KERNEL) $(ISO_DIR)/boot/myos.elf
	grub-mkrescue -o $@ $(ISO_DIR)
check: $(KERNEL)
	grub-file --is-x86-multiboot $(KERNEL)
run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO) -display none -serial stdio -no-reboot
clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR)/boot/myos.elf $(ISO)
