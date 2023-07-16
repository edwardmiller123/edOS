; In 32 bit protected mdoe we can no longer use the 16 bit bios routines.
; When the computer boots it begins in a simple VGA mode, if set in text mode
; each character cell on the screen is represented by 2 bytes in memory.
; In order to print a string then, we therefore have to set our string to the 
; corresponding bytes in the memory.
; Will use eax as a function arg to provide the offset from the start 
; of video memory to print to.

;sepcify 32 bit instructions
[bits 32]

; define constants
; location of video memory, we can move string by
; adding increments of 160
VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f

print_string_pm:
  ; we will use the ebx as our function argument 

  ; push registers to the stack
  pusha

  ; set edx to the desired of the video memory
  mov edx, VIDEO_MEMORY
  add edx, eax

  ; mark the point we wish to loop back to
  loop_print_protected:
    
    ; for accessing the individual character we still use the 8 bit registers.
    ; set al to the address of the first character stored in ebx
    mov al, [ebx]

    ; set the text attributes by storing a specific number in ah.
    ; This is similar to how we siginiifed to use the teletype intterupt in 16 bit mode.
    mov ah, WHITE_ON_BLACK

    ; check if al is 0 since this is the last character stored in the string 
    cmp al, 0

    ; if equal jump to the finished
    je finished_print_protected

    ; store the current character at the current character cell in video memory.
    mov [edx], ax

    ; increment to the next byte (character) in the string
    inc ebx
    
    ; move to the next character cell in video memory
    add edx, 2

    ;jump back to the loop label
    jmp loop_print_protected

  finished_print_protected:
    ; pop registers of the stack
    popa
    ; return i.e access the required return address from the correct register
    ret