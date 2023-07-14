; define the entry point for our kernel
[bits 32]
[extern _start]
call _start
jmp $