global switchThread

switchThread:
    push ebx
    push esi 
    push edi

    ; allow us to access function args
    ; caller pushes the function's args on the stack in reverse order
    push ebp
    mov ebp, esp


    