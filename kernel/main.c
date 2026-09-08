#include "console.h"

static void outb(unsigned short port, unsigned char value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static unsigned char inb(unsigned short port) {
    unsigned char value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void serial_init(void) {
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x80);
    outb(0x3F8 + 0, 0x03);
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x03);
    outb(0x3F8 + 2, 0xC7);
    outb(0x3F8 + 4, 0x0B);
}

static void serial_write(const char *text) {
    while (*text) {
        while ((inb(0x3F8 + 5) & 0x20) == 0) { }
        outb(0x3F8, (unsigned char)*text++);
    }
}

void kmain(void) {
    const char *message = "MyOS 0.1: booted successfully.\n";

    serial_init();
    console_init();
    console_write(message);
    serial_write("\n");
    serial_write(message);
}