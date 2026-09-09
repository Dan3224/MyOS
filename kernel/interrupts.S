.section .text
.global timer_interrupt_stub
.global keyboard_interrupt_stub
.extern ticks
.extern keyboard_interrupt_handler

timer_interrupt_stub:
    pusha
    incl ticks
    movb $0x20, %al
    outb %al, $0x20
    popa
    iret

keyboard_interrupt_stub:
    pusha
    call keyboard_interrupt_handler
    popa
    iret
