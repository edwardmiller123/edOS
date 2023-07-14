; define the entry point for our kernel
[bits 32]
[extern main]
call main
jmp $