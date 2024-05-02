global switchThread

; TODO: update the value of esp0 in the TSS when we switch thread so we can go
; between rings

; switchThread is called by the timer interrupt but never returns to the 
; handler. Instead we finish the interrupt ourselves and then set the thread function as 
; the return value.

; TODO: need to send the EOI signal when we return
switchThread:
    ; setup the base pointer to make it easy to access function args on the stack
    push ebp
    mov ebp, esp
    ; Push any extra registers we are going to use and 
    ; clean this up when the function returns. We dont push esi as we want to use
    ; it to store the return address during the stack change
    push ebx
    push edi

    ; caller pushes the function's args on the stack in reverse order
    mov edi, [ebp + 8] ; access the first argument (current thread)
    mov ebx, [ebp + 12]; access the second arg (next thread)
    
    mov [edi], esp ; store the current stack top in the old TCB
    ; the stacktop is the first element in the TCB struct so no extra addition is nessecary

    mov esp, [ebx] ; load the next threads stack into esp

    ; the new stack should be already set up with these values
    pop edi
    pop ebx
    pop ebp

    ; manually finish the interrupt since we arent returning to where we came from
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa
    add esp, 8
    sti
    iret      

    