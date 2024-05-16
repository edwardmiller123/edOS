[extern runShell]

global enterUserMode
; we enter usermode by making the cpu think we were already there
enterUserMode: 
    mov ax, (4 * 8) | 3 ; usermode data segment in the gdt with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax 
	mov fs, ax 
	mov gs, ax ; SS is handled by iret
 
	; set up the stack frame iret expects
	mov eax, esp
	push (4 * 8) | 3 ; data selector
	push eax ; current esp
	pushf ; eflags
	push (3 * 8) | 3 ; usermode code segment in gdt with bottom 2 bits set for ring 3)
	push runShell ; function address to return to
	iret