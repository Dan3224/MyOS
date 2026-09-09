#ifndef MYOS_CONSOLE_H
#define MYOS_CONSOLE_H

void console_init(void);
void console_clear(unsigned char color);
void console_set_color(unsigned char color);
void console_set_cursor(unsigned short row, unsigned short column);
void console_write_char(char character);
void console_backspace(void);
void console_write(const char *text);

#endif
