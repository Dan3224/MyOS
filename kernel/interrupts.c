#include "interrupts.h"
#include "io.h"

#define PIT_FREQUENCY 100

struct idt_entry {
    unsigned short offset_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char attributes;
    unsigned short offset_high;
} __attribute__((packed));

struct idt_pointer {
    unsigned short size;
    unsigned int address;
} __attribute__((packed));

extern void timer_interrupt_stub(void);
extern void keyboard_interrupt_stub(void);

static struct idt_entry idt[256];
volatile unsigned long ticks = 0;

static void set_gate(unsigned char vector, unsigned int handler, unsigned short selector) {
    idt[vector].offset_low = handler & 0xFFFF;
    idt[vector].selector = selector;
    idt[vector].zero = 0;
    idt[vector].attributes = 0x8E;
    idt[vector].offset_high = (handler >> 16) & 0xFFFF;
}

static void pic_init(void) {
    outb(0x20, 0x11);
    io_wait();
    outb(0xA0, 0x11);
    io_wait();
    outb(0x21, 0x20);
    io_wait();
    outb(0xA1, 0x28);
    io_wait();
    outb(0x21, 0x04);
    io_wait();
    outb(0xA1, 0x02);
    io_wait();
    outb(0x21, 0x01);
    io_wait();
    outb(0xA1, 0x01);
    io_wait();

    outb(0x21, 0xFC);
    io_wait();
    outb(0xA1, 0xFF);
    io_wait();
}

static void pit_init(void) {
    unsigned short divisor = 1193182 / PIT_FREQUENCY;

    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}

void interrupts_init(void) {
    struct idt_pointer pointer;
    unsigned short code_selector;

    for (unsigned int index = 0; index < 256; index++) {
        idt[index].offset_low = 0;
        idt[index].selector = 0;
        idt[index].zero = 0;
        idt[index].attributes = 0;
        idt[index].offset_high = 0;
    }

    __asm__ volatile ("mov %%cs, %0" : "=r"(code_selector));
    set_gate(32, (unsigned int)timer_interrupt_stub, code_selector);
    set_gate(33, (unsigned int)keyboard_interrupt_stub, code_selector);
    pointer.size = sizeof(idt) - 1;
    pointer.address = (unsigned int)idt;

    __asm__ volatile ("lidtl (%0)" : : "r"(&pointer));
    pic_init();
    pit_init();
    __asm__ volatile ("sti");
}

unsigned long timer_ticks(void) {
    return ticks;
}
