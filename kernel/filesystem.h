#ifndef MYOS_FILESYSTEM_H
#define MYOS_FILESYSTEM_H

void filesystem_init(void);
void filesystem_list(void);
int filesystem_read(const char *name);
int filesystem_write(const char *name, const char *contents);
unsigned int filesystem_count(void);

#endif
