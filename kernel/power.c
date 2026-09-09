#include "io.h"
#include "power.h"

void system_reboot(void) {
    while (inb(0x64) & 0x02) { }
    outb(0x64, 0xFE);

    while (1) {
        __asm__ volatile ("hlt");
    }
}
