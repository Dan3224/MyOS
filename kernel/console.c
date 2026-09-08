#include "console.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR 0x0F

static unsigned short *const vga_buffer = (unsigned short *)0xB8000;
static unsigned short cursor = 0;

void console_init(void) {
    for (unsigned short index = 0; index < VGA_WIDTH * VGA_HEIGHT; index++) {
        vga_buffer[index] = ((unsigned short)VGA_COLOR << 8) | ' ';
    }
    cursor = 0;
}

void console_write_char(char character) {
    if (character == '\n') {
        cursor = (cursor / VGA_WIDTH + 1) * VGA_WIDTH;
    } else {
        vga_buffer[cursor++] = ((unsigned short)VGA_COLOR << 8) | (unsigned char)character;
    }
    if (cursor >= VGA_WIDTH * VGA_HEIGHT) {
        cursor = 0;
    }
}

void console_write(const char *text) {
    while (*text) {
        console_write_char(*text++);
    }
}