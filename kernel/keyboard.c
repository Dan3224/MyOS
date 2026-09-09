#include "io.h"
#include "keyboard.h"

#define KEYBOARD_BUFFER_SIZE 64

static volatile char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile unsigned char buffer_head = 0;
static volatile unsigned char buffer_tail = 0;

static char decode_scancode(unsigned char code) {
    switch (code) {
        case 0x02: return '1'; case 0x03: return '2'; case 0x04: return '3';
        case 0x05: return '4'; case 0x06: return '5'; case 0x07: return '6';
        case 0x08: return '7'; case 0x09: return '8'; case 0x0A: return '9';
        case 0x0B: return '0'; case 0x0E: return '\b';

        case 0x10: return 'q'; case 0x11: return 'w'; case 0x12: return 'e';
        case 0x13: return 'r'; case 0x14: return 't'; case 0x15: return 'y';
        case 0x16: return 'u'; case 0x17: return 'i'; case 0x18: return 'o';
        case 0x19: return 'p';

        case 0x1E: return 'a'; case 0x1F: return 's'; case 0x20: return 'd';
        case 0x21: return 'f'; case 0x22: return 'g'; case 0x23: return 'h';
        case 0x24: return 'j'; case 0x25: return 'k'; case 0x26: return 'l';

        case 0x2C: return 'z'; case 0x2D: return 'x'; case 0x2E: return 'c';
        case 0x2F: return 'v'; case 0x30: return 'b'; case 0x31: return 'n';
        case 0x32: return 'm';

        case 0x1C: return '\n';
        case 0x39: return ' ';
        default: return 0;
    }
}

void keyboard_interrupt_handler(void) {
    unsigned char code = inb(0x60);
    char character;

    if (code & 0x80) {
        outb(0x20, 0x20);
        return;
    }

    character = decode_scancode(code);
    if (character) {
        unsigned char next_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;

        if (next_head != buffer_tail) {
            keyboard_buffer[buffer_head] = character;
            buffer_head = next_head;
        }
    }

    outb(0x20, 0x20);
}

char keyboard_read_char(void) {
    char character;

    while (buffer_head == buffer_tail) {
        __asm__ volatile ("hlt");
    }

    character = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return character;
}

char keyboard_poll_char(void) {
    char character;

    if (buffer_head == buffer_tail) {
        return 0;
    }

    character = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return character;
}
