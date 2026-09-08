#include "console.h"
#include "io.h"
#include "keyboard.h"

static void serial_init(void) {
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x80);
    outb(0x3F8 + 0, 0x03);
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x03);
    outb(0x3F8 + 2, 0xC7);
    outb(0x3F8 + 4, 0x0B);
}

static void serial_write_char(char character) {
    while ((inb(0x3F8 + 5) & 0x20) == 0) { }
    outb(0x3F8, (unsigned char)character);
}

static void serial_write(const char *text) {
    while (*text) {
        serial_write_char(*text++);
    }
}

void kmain(void) {
    const char *message = "MyOS 0.1: booted successfully.\nType on the keyboard: ";

    serial_init();
    console_init();
    console_write(message);
    serial_write("\n");
    serial_write(message);

    while (1) {
        char character = keyboard_read_char();
        if (character) {
            console_write_char(character);
            serial_write_char(character);
        }
    }
}