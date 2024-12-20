[extern isrHandler]
[extern irqHandler]
[extern syscallHandler]

; Common ISR code
; this is how we "interrupt" the cpu and then continue on our
; merry way.
isr_common_stub:
    ; 1. Save CPU state
	pusha ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
	mov ax, ds ; Lower 16-bits of eax = ds.
	push eax ; save the data segment descriptor
	mov ax, 0x10  ; kernel data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

    push esp
	
    ; 2. Call C handler
	call isrHandler

    pop esp
	
    ; 3. Restore state
	pop eax 
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	popa
	add esp, 8 ; Cleans up the pushed error code and pushed ISR number
	iret ; pops 5 things at once: EIP, CS, EFLAGS, (ESP, and SS if coming from user mode)
    ; sti isnt needed as iret sets the IF bit in EFLAGS

; very similar to the isr routine but the restore is slightly different
irq_common_stub:
    pusha 

    ; zero eax so we dont corrupt ds with whatever was left there from before
    ; e.g a function return value
    xor eax, eax

	mov ax, ds 
	push eax 
	mov ax, 0x10 
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

    ; save esp so we can easily access the caller stack in the irqHandler
    push esp

    call irqHandler

    ; either pop the original value or a new value if a thread switch has occurred
    pop esp

    pop ebx  ; To differentiate from the isr we pop ebx.
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa
    add esp, 8 ; Cleans up the pushed error code and pushed IRQ number
    iret 

; This could just be the irq stub however for debuging its easier to keep the syscalls seperate
syscall_stub:
    pusha
    ; zero eax so we dont corrupt ds with whatever was left there from before
    ; e.g a function return value
    xor eax, eax

    mov ax, ds 
	push eax 
	mov ax, 0x10 
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

    push esp

    call syscallHandler

    pop esp

    pop ebx 
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    pop edi
    pop esi
    pop ebp
    add esp, 4 ; we skip poping the stack pointer as it is modified anyway by each pop instruction
    pop ebx
    pop edx
    pop ecx
    add esp, 4 ; (pop eax) the return value from syscallHandler is stored in eax so we skip poping a value off the stack to avoid overwriting it
    ; this allows us to read it from eax later.
    add esp, 8
    iret

; We don't get information about which interrupt was called
; when the handler is run, so we will need to have a different handler
; for every interrupt.
; Some interrupts push an error code onto the stack but others
; don't, so we will push a dummy error code for those which don't, so that
; we have a consistent stack for all of them.

; Define the ISR's
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31

; define the irqs
global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15


; 0: Divide By Zero Exception
isr0:
    cli
    push dword 0 ; These 0 dwords are the dummy error codes for the interrupts that dont push any
    push dword 0
    jmp isr_common_stub

; 1: Debug Exception
isr1:
    cli
    push dword 0
    push dword 1
    jmp isr_common_stub

; 2: Non Maskable Interrupt Exception
isr2:
    cli
    push dword 0
    push dword 2
    jmp isr_common_stub

; 3: Int 3 Exception
isr3:
    cli
    push dword 0
    push dword 3
    jmp isr_common_stub

; 4: INTO Exception
isr4:
    cli
    push dword 0
    push dword 4
    jmp isr_common_stub

; 5: Out of Bounds Exception
isr5:
    cli
    push dword 0
    push dword 5
    jmp isr_common_stub

; 6: Invalid Opcode Exception
isr6:
    cli
    push dword 0
    push dword 6
    jmp isr_common_stub

; 7: Coprocessor Not Available Exception
isr7:
    cli
    push dword 0
    push dword 7
    jmp isr_common_stub

; 8: Double Fault Exception (With Error Code!)
isr8:
    cli
    push dword 8
    jmp isr_common_stub

; 9: Coprocessor Segment Overrun Exception
isr9:
    cli
    push dword 0
    push dword 9
    jmp isr_common_stub

; 10: Bad TSS Exception (With Error Code!)
isr10:
    cli
    push dword 10
    jmp isr_common_stub

; 11: Segment Not Present Exception (With Error Code!)
isr11:
    cli
    push dword 11
    jmp isr_common_stub

; 12: Stack Fault Exception (With Error Code!)
isr12:
    cli
    push dword 12
    jmp isr_common_stub

; 13: General Protection Fault Exception (With Error Code!)
isr13:
    cli
    push dword 13
    jmp isr_common_stub

; 14: Page Fault Exception (With Error Code!)
isr14:
    cli
    push dword 14
    jmp isr_common_stub

; 15: Reserved Exception
isr15:
    cli
    push dword 0
    push dword 15
    jmp isr_common_stub

; 16: Floating Point Exception
isr16:
    cli
    push dword 0
    push dword 16
    jmp isr_common_stub

; 17: Alignment Check Exception
isr17:
    cli
    push dword 0
    push dword 17
    jmp isr_common_stub

; 18: Machine Check Exception
isr18:
    cli
    push dword 0
    push dword 18
    jmp isr_common_stub

; 19: Reserved
isr19:
    cli
    push dword 0
    push dword 19
    jmp isr_common_stub

; 20: Reserved
isr20:
    cli
    push dword 0
    push dword 20
    jmp isr_common_stub

; 21: Reserved
isr21:
    cli
    push dword 0
    push dword 21
    jmp isr_common_stub

; 22: Reserved
isr22:
    cli
    push dword 0
    push dword 22
    jmp isr_common_stub

; 23: Reserved
isr23:
    cli
    push dword 0
    push dword 23
    jmp isr_common_stub

; 24: Reserved
isr24:
    cli
    push dword 0
    push dword 24
    jmp isr_common_stub

; 25: Reserved
isr25:
    cli
    push dword 0
    push dword 25
    jmp isr_common_stub

; 26: Reserved
isr26:
    cli
    push dword 0
    push dword 26
    jmp isr_common_stub

; 27: Reserved
isr27:
    cli
    push dword 0
    push dword 27
    jmp isr_common_stub

; 28: Reserved
isr28:
    cli
    push dword 0
    push dword 28
    jmp isr_common_stub

; 29: Reserved
isr29:
    cli
    push dword 0
    push dword 29
    jmp isr_common_stub

; 30: Reserved
isr30:
    cli
    push dword 0
    push dword 30
    jmp isr_common_stub

; 31: Reserved
isr31:
    cli
    push dword 0
    push dword 31
    jmp isr_common_stub


; IRQ handlers
; timer
irq0:
	cli
	push dword 0
	push dword 32
	jmp irq_common_stub

; keyboard
irq1:
	cli
	push dword 1
	push dword 33
	jmp irq_common_stub

; slave pic line
irq2:
	cli
	push dword 2
	push dword 34
	jmp irq_common_stub

irq3:
	cli
	push dword 3
	push dword 35
	jmp irq_common_stub

irq4:
	cli
	push dword 4
	push dword 36
	jmp irq_common_stub

irq5:
	cli
	push dword 5
	push dword 37
	jmp irq_common_stub

irq6:
	cli
	push dword 6
	push dword 38
	jmp irq_common_stub

irq7:
	cli
	push dword 7
	push dword 39
	jmp irq_common_stub

irq8:
	cli
	push dword 8
	push dword 40
	jmp irq_common_stub

irq9:
	cli
	push dword 9
	push dword 41
	jmp irq_common_stub

irq10:
	cli
	push dword 10
	push dword 42
	jmp irq_common_stub

irq11:
	cli
	push dword 11
	push dword 43
	jmp irq_common_stub

irq12:
	cli
	push dword 12
	push dword 44
	jmp irq_common_stub

irq13:
	cli
	push dword 13
	push dword 45
	jmp irq_common_stub

irq14:
	cli
	push dword 14
	push dword 46
	jmp irq_common_stub

; We use irq 15 for system calls
irq15:
	cli
	push dword 15
	push dword 47
	jmp syscall_stub