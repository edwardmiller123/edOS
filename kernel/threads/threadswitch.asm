global switchThread

; TODO: update the value of esp0 in the TSS when we switch
switchThread:
    ; setup the base pointer to make it easy to access function args on the stack
    push ebp
    mov ebp, esp
    ; Push any extra registers we are going to use and 
    ; clean this up when the function returns.
    push ebx
    push esi
    push edi

    ; caller pushes the function's args on the stack in reverse order
    mov edi, [ebp + 8] ; access the first argument (current thread)
    mov ebx, [ebp + 12]; access the second arg (next thread)

    mov ecx, 5
    mov [edi + 8], ecx
    
    mov [edi], esp ; store the current stack top in the old TCB
    ; the stacktop is the first element in the TCB struct so no extra addition is nessecary

    mov esp, [ebx] ; load the next threads stack into esp

    pop edi
    pop esi
    pop ebx

    ret      

    