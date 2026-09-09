#ifndef MYOS_KEYBOARD_H
#define MYOS_KEYBOARD_H

void keyboard_interrupt_handler(void);
char keyboard_read_char(void);
char keyboard_poll_char(void);

#endif
