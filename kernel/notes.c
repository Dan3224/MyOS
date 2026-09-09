#include "filesystem.h"
#include "notes.h"

#define NOTE_MAX 128

static char note[NOTE_MAX];
static unsigned int note_length;
static int editing;

static void copy_text(const char *source) {
    note_length = 0;

    while (source && *source && note_length < NOTE_MAX - 1) {
        note[note_length++] = *source++;
    }

    note[note_length] = 0;
}

void notes_init(void) {
    copy_text(filesystem_contents("notes"));
    editing = 0;
}

void notes_start_edit(void) {
    editing = 1;
}

int notes_handle_input(char character) {
    if (!editing) {
        return 0;
    }

    if (character == '\b') {
        if (note_length > 0) {
            note[--note_length] = 0;
        }
        return 0;
    }

    if (character == '\n') {
        filesystem_write("notes", note);
        editing = 0;
        return 1;
    }

    if (character >= ' ' && character <= '~' && note_length < NOTE_MAX - 1) {
        note[note_length++] = character;
        note[note_length] = 0;
    }

    return 0;
}

int notes_is_editing(void) {
    return editing;
}

const char *notes_text(void) {
    return note;
}
