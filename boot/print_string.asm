print_string:
  ; push registers to the stack 
  push bx
  ; prepare interrupt
  mov ah, 0x0e
  ; mark the point we wish to loop back to
  loop_print_real:
    ; set al to the address stored in bx
    mov al, [bx]

     ; move to next byte of bx
    inc bx

    ; check if al is 0 since this is the last character stored in the string 
    cmp al, 0

    ; if equal jump to the finished
    je finished_print_real

    ; trigger the bios interrupt
    int 0x10

    ;jump back to the loop label
    jmp loop_print_real

  finished_print_real:
    ; pop registers of the stack
    pop bx
    ; return i.e access the required return address from the correct register
    ret
