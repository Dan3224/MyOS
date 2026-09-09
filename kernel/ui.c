#include "console.h"
#include "filesystem.h"
#include "interrupts.h"
#include "ui.h"

#define COLOR_BACKGROUND 0x17
#define COLOR_HEADER 0x1F
#define COLOR_TITLE 0x1E
#define COLOR_PANEL 0x70
#define COLOR_TEXT 0x1F
#define COLOR_HINT 0x1B

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

static void write_at(unsigned short row, unsigned short column, unsigned char color,
                     const char *text) {
    console_set_cursor(row, column);
    console_set_color(color);
    console_write(text);
}

static void fill_row(unsigned short row, unsigned char color) {
    console_set_cursor(row, 0);
    console_set_color(color);
    for (unsigned short column = 0; column < 80; column++) {
        console_write_char(' ');
    }
}

static void draw_frame(const char *active_page) {
    console_clear(COLOR_BACKGROUND);

    fill_row(0, COLOR_HEADER);
    write_at(0, 2, COLOR_HEADER, "MyOS 0.5  |  Visual Shell");
    write_at(0, 58, COLOR_HEADER, "uptime ");
    write_number(timer_ticks() / 100);
    write_at(0, 73, COLOR_HEADER, "sec");

    fill_row(2, COLOR_PANEL);
    write_at(2, 2, COLOR_PANEL, "[H] Home");
    write_at(2, 15, COLOR_PANEL, "[F] Files");
    write_at(2, 30, COLOR_PANEL, "[S] System");
    write_at(2, 47, COLOR_PANEL, "[T] Terminal");
    write_at(2, 66, COLOR_PANEL, "Active: ");
    write_at(2, 74, COLOR_TITLE, active_page);

    fill_row(23, COLOR_HINT);
    write_at(23, 2, COLOR_HINT,
             "H Home   F Files   S System   T Terminal   Q Reboot");
    fill_row(24, COLOR_HEADER);
    write_at(24, 2, COLOR_HEADER, "MyOS 0.5 - keyboard-driven visual interface");
}

void ui_show_home(void) {
    draw_frame("HOME");
    write_at(5, 5, COLOR_TITLE, "Welcome to MyOS");
    write_at(7, 5, COLOR_TEXT, "Your system is running from a bootable image.");
    write_at(9, 5, COLOR_TEXT, "Open a section with one key:");
    write_at(11, 9, COLOR_TITLE, "[F] Files");
    write_at(12, 9, COLOR_TEXT, "Inspect the virtual filesystem.");
    write_at(14, 9, COLOR_TITLE, "[T] Terminal");
    write_at(15, 9, COLOR_TEXT, "Use commands: help, ls, cat, write, sysinfo.");
    write_at(17, 9, COLOR_TITLE, "[S] System");
    write_at(18, 9, COLOR_TEXT, "See the current MyOS status.");
}

void ui_show_files(void) {
    draw_frame("FILES");
    write_at(5, 5, COLOR_TITLE, "Virtual filesystem");
    write_at(7, 5, COLOR_TEXT, "Files currently stored: ");
    write_number(filesystem_count());
    write_at(9, 5, COLOR_TEXT, "Built-in files: readme, version");
    write_at(11, 5, COLOR_TEXT, "To view or change files, open Terminal [T].");
    write_at(13, 5, COLOR_HINT, "Examples: ls   cat readme   write note hello");
}

void ui_show_system(void) {
    draw_frame("SYSTEM");
    write_at(5, 5, COLOR_TITLE, "MyOS system status");
    write_at(7, 5, COLOR_TEXT, "Release: MyOS 0.5 Visual Shell");
    write_at(9, 5, COLOR_TEXT, "Uptime: ");
    write_number(timer_ticks() / 100);
    write_at(9, 17, COLOR_TEXT, "seconds");
    write_at(11, 5, COLOR_TEXT, "Virtual files: ");
    write_number(filesystem_count());
    write_at(13, 5, COLOR_TEXT, "Input: PS/2 keyboard interrupt driver");
    write_at(15, 5, COLOR_TEXT, "Display: VGA text mode, 80 x 25, color");
    write_at(17, 5, COLOR_HINT, "Press T to return to the command terminal.");
}

void ui_show_terminal(void) {
    console_clear(0x0F);
    write_at(0, 0, COLOR_HEADER, " MyOS 0.5 Terminal ");
    write_at(2, 0, 0x0F, "Type help for commands. Type home to return to Visual Shell.\n");
}
