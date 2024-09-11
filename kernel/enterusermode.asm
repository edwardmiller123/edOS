[extern initUserSpace]

global enterUserMode
; we enter usermode by making the cpu think we were already there
enterUserMode: 
    mov ax, 0x23 ; usermode data segment in the gdt with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax 
	mov fs, ax 
	mov gs, ax ; SS is handled by iret
 
	; set up the stack frame iret expects
	mov eax, esp
	push 0x23 ; data selector
	push eax ; current esp
	pushfd ; eflags
	push 0x1b ; usermode code segment in gdt with bottom 2 bits set for ring 3
	push initUserSpace ; function address to return to
	iret