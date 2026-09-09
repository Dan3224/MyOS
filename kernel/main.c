#include "console.h"
#include "filesystem.h"
#include "interrupts.h"
#include "io.h"
#include "keyboard.h"
#include "power.h"
#include "ui.h"

#define COMMAND_MAX 160

static char command[COMMAND_MAX];
static unsigned int command_length = 0;

enum screen_mode {
    SCREEN_HOME,
    SCREEN_FILES,
    SCREEN_SYSTEM,
    SCREEN_TERMINAL
};

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

static void write_filesystem_error(void) {
    console_write("Filesystem: invalid name, file missing, or storage full.\n");
}

static void write_number(unsigned long value) {
    char digits[11];
    unsigned int length = 0;

    if (value == 0) {
        console_write_char('0');
        return;
    }

    while (value > 0) {
        digits[length++] = '0' + (value % 10);
        value /= 10;
    }

    while (length > 0) {
        console_write_char(digits[--length]);
    }
}

static int run_command(void) {
    if (equals(command, "help")) {
        console_write("Commands: help, about, clear, echo, ls, cat, write, sysinfo, uptime, reboot, home\n");
    } else if (equals(command, "about")) {
        console_write("MyOS 0.5 - a small open source OS prototype with a visual shell.\n");
    } else if (equals(command, "clear")) {
        ui_show_terminal();
    } else if (starts_with(command, "echo ")) {
        console_write(command + 5);
        console_write("\n");
    } else if (equals(command, "ls")) {
        filesystem_list();
    } else if (starts_with(command, "cat ")) {
        if (!filesystem_read(command + 4)) {
            write_filesystem_error();
        }
    } else if (starts_with(command, "write ")) {
        char *name = command + 6;
        char *contents = name;

        while (*contents && *contents != ' ') {
            contents++;
        }

        if (*contents == 0) {
            console_write("Usage: write <name> <text>\n");
        } else {
            *contents++ = 0;
            while (*contents == ' ') {
                contents++;
            }

            if (!filesystem_write(name, contents)) {
                write_filesystem_error();
            }
        }
    } else if (equals(command, "sysinfo")) {
        console_write("MyOS 0.4 Core System\nUptime: ");
        write_number(timer_ticks() / 100);
        console_write(" seconds\nFiles: ");
        write_number(filesystem_count());
        console_write("\n");
    } else if (equals(command, "home")) {
        ui_show_home();
        return 1;
    } else if (equals(command, "uptime")) {
        console_write("Uptime: ");
        write_number(timer_ticks() / 100);
        console_write(" seconds\n");
    } else if (equals(command, "reboot")) {
        console_write("Rebooting MyOS...\n");
        system_reboot();
    } else if (command_length) {
        console_write("Unknown command. Type help.\n");
    }

    return 0;
}

void kmain(void) {
    enum screen_mode mode = SCREEN_HOME;

    serial_init();
    console_init();
    filesystem_init();
    interrupts_init();

    ui_show_home();

    while (1) {
        char character = keyboard_read_char();

        if (!character) {
            continue;
        }

        if (mode != SCREEN_TERMINAL) {
            if (character == 'h') {
                mode = SCREEN_HOME;
                ui_show_home();
            } else if (character == 'f') {
                mode = SCREEN_FILES;
                ui_show_files();
            } else if (character == 's') {
                mode = SCREEN_SYSTEM;
                ui_show_system();
            } else if (character == 't') {
                mode = SCREEN_TERMINAL;
                command_length = 0;
                ui_show_terminal();
                prompt();
            } else if (character == 'q') {
                system_reboot();
            }
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
            if (run_command()) {
                command_length = 0;
                mode = SCREEN_HOME;
                continue;
            }
            command_length = 0;
            prompt();
        } else if (command_length < COMMAND_MAX - 1) {
            command[command_length++] = character;
            console_write_char(character);
            serial_write_char(character);
        }
    }
}
