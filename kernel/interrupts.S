.section .text
.global timer_interrupt_stub
.global keyboard_interrupt_stub
.global divide_error_stub
.global invalid_opcode_stub
.global general_protection_stub
.global page_fault_stub
.extern ticks
.extern keyboard_interrupt_handler
.extern graphics_show_crash

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

divide_error_stub:
    pushl $0
    pushl $0
    jmp exception_common

invalid_opcode_stub:
    pushl $0
    pushl $6
    jmp exception_common

general_protection_stub:
    pushl $13
    jmp exception_common

page_fault_stub:
    pushl $14

exception_common:
    cli
    pusha
    movl 32(%esp), %eax
    pushl %eax
    call graphics_show_crash
    addl $4, %esp

exception_halt:
    hlt
    jmp exception_halt
