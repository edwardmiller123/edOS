
; This is a two stage loader which allows us to make space for kernel without
; overwriting any boot code or hitting legacy memory addressing issues.

;any memory offsets will be from this point (location of boot sector)
[org 0x7C00] ; loaded at 31kb mark

; create an alias for the memory offset which we will load our kernel into.
STAGE_2_OFFSET equ 0x600 ; load at 31.5kb mark

; set the segment registers as the bios doesnt initialise them itself
mov ax, 0x0
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax
mov sp, ax

;set the stack position (sp);
mov bp, 0x7C00 ; init stack at 31kb (grows downwards)
mov sp, bp

; call print_string in 16 bit mode so we know everythings working
mov bx, MSG_STAGE_1_REAL_MODE
call print_string

; now we read stage 2 from the disk
call load_stage_2

jmp STAGE_2_OFFSET

  ; Hang forever (not really nessecary since we never return from the above)
jmp $

%include "print_string.asm"
%include "disk_load.asm"

[bits 16]
; load our second stage
load_stage_2:
  ; print a message so we know we are loading the second stage
  mov bx, MSG_LOAD_STAGE_2
  call print_string

  ; we want to load stage 2 to the address stored at STAGE_2_OFFSET
  mov bx, STAGE_2_OFFSET

  ; this is the number of sectors we want to load from the boot media 
  ; we need to ensure not to overwrite anything we shouldn't.
  ; We load just 1 sector as our second stage is also just 512 bytes
  mov dh, 1

  ; store the boot_drive address back in dl
  mov dl, [BOOT_DRIVE]

  ; make the call to read the disk
  call disk_load_stage_1
  ret


; Global variables
BOOT_DRIVE db 0
; nasm interprets 13 as carriage return and 10 as line feed
; so this is equivalent to printing '\n message'
; we dont add this for the 32 bit string as we are directly
; accessing video memory rather than using the bios.
MSG_STAGE_1_REAL_MODE db 13,10,"Booted in 16-bit real mode. Stage 1.", 0
MSG_LOAD_STAGE_2 db 13,10,"Loading stage 2", 0

; magic number. Where the magic begins.
times 510-($-$$) db 0
dw 0xaa55