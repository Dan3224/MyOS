.section .multiboot
.align 4
.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set VIDEO,    1<<2
.set FLAGS,    ALIGN | MEMINFO | VIDEO
.long 0x1BADB002
.long FLAGS
.long -(0x1BADB002 + FLAGS)
.long 0
.long 0
.long 0
.long 0
.long 0
.long 0
.long 1024
.long 768
.long 32

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global _start
.extern kmain
_start:
    mov $stack_top, %esp
    push %ebx
    push %eax
    call kmain
    add $8, %esp

halt:
    cli
    hlt
    jmp halt
