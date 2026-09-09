.section .text
.global timer_interrupt_stub
.extern ticks

timer_interrupt_stub:
    pusha
    incl ticks
    movb $0x20, %al
    outb %al, $0x20
    popa
    iret
