#include "console.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR 0x0F

static unsigned short *const vga_buffer = (unsigned short *)0xB8000;
static unsigned short cursor = 0;
static unsigned char current_color = VGA_COLOR;

void console_init(void) {
    console_clear(VGA_COLOR);
}

void console_clear(unsigned char color) {
    current_color = color;
    for (unsigned short index = 0; index < VGA_WIDTH * VGA_HEIGHT; index++) {
        vga_buffer[index] = ((unsigned short)current_color << 8) | ' ';
    }
    cursor = 0;
}

void console_set_color(unsigned char color) {
    current_color = color;
}

void console_set_cursor(unsigned short row, unsigned short column) {
    if (row >= VGA_HEIGHT || column >= VGA_WIDTH) {
        return;
    }

    cursor = row * VGA_WIDTH + column;
}

void console_write_char(char character) {
    if (character == '\n') {
        cursor = (cursor / VGA_WIDTH + 1) * VGA_WIDTH;
    } else {
        vga_buffer[cursor++] = ((unsigned short)current_color << 8) | (unsigned char)character;
    }
    if (cursor >= VGA_WIDTH * VGA_HEIGHT) {
        cursor = 0;
    }
}

void console_backspace(void) {
    if (cursor == 0) {
        return;
    }

    cursor--;
    vga_buffer[cursor] = ((unsigned short)current_color << 8) | ' ';
}

void console_write(const char *text) {
    while (*text) {
        console_write_char(*text++);
    }
}
