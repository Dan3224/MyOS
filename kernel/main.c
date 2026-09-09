#include "console.h"
#include "io.h"
#include "keyboard.h"

#define COMMAND_MAX 32

static char command[COMMAND_MAX];
static unsigned int command_length = 0;

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

static int equals(const char *left, const char *right) {
    while (*left && *right) {
        if (*left++ != *right++) {
            return 0;
        }
    }

    return *left == *right;
}

static int starts_with(const char *text, const char *prefix) {
    while (*prefix) {
        if (*text++ != *prefix++) {
            return 0;
        }
    }

    return 1;
}

static void prompt(void) {
    console_write("MyOS> ");
}

static void run_command(void) {
    if (equals(command, "help")) {
        console_write("Commands: help, about, clear, echo <text>\n");
    } else if (equals(command, "about")) {
        console_write("MyOS 0.1 - a small open source OS prototype.\n");
    } else if (equals(command, "clear")) {
        console_init();
    } else if (starts_with(command, "echo ")) {
        console_write(command + 5);
        console_write("\n");
    } else if (command_length) {
        console_write("Unknown command. Type help.\n");
    }
}

void kmain(void) {
    serial_init();
    console_init();

    console_write("MyOS 0.1: booted successfully.\n");
    console_write("Type help for commands.\n");
    prompt();

    while (1) {
        char character = keyboard_read_char();

        if (!character) {
            continue;
        }

        if (character == '\b') {
            if (command_length > 0) {
                command_length--;
                console_backspace();
            }
            continue;
        }

        if (character == '\n') {
            console_write_char(character);
            serial_write_char(character);

            command[command_length] = 0;
            run_command();
            command_length = 0;
            prompt();
        } else if (command_length < COMMAND_MAX - 1) {
            command[command_length++] = character;
            console_write_char(character);
            serial_write_char(character);
        }
    }
}