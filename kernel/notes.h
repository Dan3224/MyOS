#ifndef MYOS_NOTES_H
#define MYOS_NOTES_H

void notes_init(void);
void notes_start_edit(void);
int notes_handle_input(char character);
int notes_is_editing(void);
const char *notes_text(void);

#endif
