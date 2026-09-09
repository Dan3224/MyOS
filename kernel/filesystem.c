#include "console.h"
#include "filesystem.h"

#define MAX_FILES 8
#define FILE_NAME_MAX 16
#define FILE_CONTENT_MAX 128

struct virtual_file {
    char name[FILE_NAME_MAX];
    char contents[FILE_CONTENT_MAX];
    int used;
};

static struct virtual_file files[MAX_FILES];

static int equals(const char *left, const char *right) {
    while (*left && *right) {
        if (*left++ != *right++) {
            return 0;
        }
    }

    return *left == *right;
}

static unsigned int length_of(const char *text) {
    unsigned int length = 0;

    while (text[length]) {
        length++;
    }

    return length;
}

static void copy_text(char *destination, const char *source) {
    while (*source) {
        *destination++ = *source++;
    }

    *destination = 0;
}

static struct virtual_file *find_file(const char *name) {
    for (unsigned int index = 0; index < MAX_FILES; index++) {
        if (files[index].used && equals(files[index].name, name)) {
            return &files[index];
        }
    }

    return 0;
}

void filesystem_init(void) {
    filesystem_write("readme", "Welcome to the MyOS virtual filesystem.");
    filesystem_write("version", "MyOS 0.4 Core System");
}

void filesystem_list(void) {
    unsigned int count = 0;

    console_write("Files:\n");
    for (unsigned int index = 0; index < MAX_FILES; index++) {
        if (files[index].used) {
            console_write("- ");
            console_write(files[index].name);
            console_write("\n");
            count++;
        }
    }

    if (count == 0) {
        console_write("(empty)\n");
    }
}

int filesystem_read(const char *name) {
    struct virtual_file *file = find_file(name);

    if (!file) {
        return 0;
    }

    console_write(file->contents);
    console_write("\n");
    return 1;
}

const char *filesystem_contents(const char *name) {
    struct virtual_file *file = find_file(name);

    if (!file) {
        return 0;
    }

    return file->contents;
}

int filesystem_write(const char *name, const char *contents) {
    struct virtual_file *file = find_file(name);

    if (length_of(name) == 0 || length_of(name) >= FILE_NAME_MAX ||
        length_of(contents) >= FILE_CONTENT_MAX) {
        return 0;
    }

    if (!file) {
        for (unsigned int index = 0; index < MAX_FILES; index++) {
            if (!files[index].used) {
                file = &files[index];
                file->used = 1;
                break;
            }
        }
    }

    if (!file) {
        return 0;
    }

    copy_text(file->name, name);
    copy_text(file->contents, contents);
    return 1;
}

unsigned int filesystem_count(void) {
    unsigned int count = 0;

    for (unsigned int index = 0; index < MAX_FILES; index++) {
        if (files[index].used) {
            count++;
        }
    }

    return count;
}
