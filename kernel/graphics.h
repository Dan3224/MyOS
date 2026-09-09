#ifndef MYOS_GRAPHICS_H
#define MYOS_GRAPHICS_H

int graphics_init(unsigned int multiboot_magic, unsigned int multiboot_info);
void graphics_show_boot_stage(unsigned int stage);
void graphics_show_home(void);
void graphics_show_files(unsigned int file_count);
void graphics_show_system(unsigned long uptime_seconds, unsigned int file_count);
void graphics_show_apps(void);
void graphics_show_notes(void);
void graphics_show_crash(unsigned int vector);
void graphics_update_heartbeat(unsigned long ticks);

#endif
