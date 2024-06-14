
;any memory offsets will be from this point (location of boot sector)
[org 0x600] ; loaded at 1.5kb mark

; create an alias for the memory offset which we will load our kernel into.
KERNEL_OFFSET equ 0x900 ; load at 2.25kb mark

; set the segment registers as the bios doesnt initialise them itself
mov ax, 0x0
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax
mov sp, ax

; set the stack position (sp);
mov bp, 0x600 ; init stack at 31kb (grows downwards)
mov sp, bp

; bios stores our boot drive in dl so we store its contense at the address 
; stored in the BOOT_DRIVE label so we can remember it.
mov [BOOT_DRIVE], dl

; call print_string in 16 bit mode so we know everythings working
mov bx, MSG_STAGE_2_REAL_MODE
call print_string

; now we read our kernel from the disk
call load_kernel

; now make the switch to 32 bit mode
call switch_to_pm

; even though we never return from the above function, we loop forever here
; as a way of catching errors.
jmp $

%include "print_string.asm"
%include "print_string_pm.asm"
%include "disk_load.asm"
%include "gdt.asm"
%include "switch_to_pm.asm"

[bits 16]
; load our kernel
load_kernel:
  ; print a message so we know we are loading the kernel
  mov bx, MSG_LOAD_KERNEL
  call print_string

  ; we want to load the kernel to the address stored at KERNEL_OFFSET
  mov bx, KERNEL_OFFSET

  ; this is the number of sectors we want to load from the boot media 
  ; we need to ensure not to overwrite anything we shouldnt.
  mov dh, 100

  ; store the boot_drive address back in dl
  mov dl, [BOOT_DRIVE]

  ; make the call to read the disk
  call disk_load_stage_2
  ret

[bits 32]
; this is where we jump to after we have made the switch in switch_to_pm
BEGIN_PM:
  ; we can now execute our 32 bit code here.

  ; print message so we know we have landed safely
  mov ebx, MSG_PROTECTED_MODE
  ; location to print to. We provide the offset 
  ; from the start of video memory, we can move string by
  ; adding increments of 160
  mov eax, 2400
  call print_string_pm

  ; load the tss to allow for pivilege level changes (and context switches but
  ; we arent using it for that here)
  mov ax, 0x28 ; address of the tss descriptor in the gdt
  ltr ax

  ; Finally jump to the memory address of our loaded kernel.
  jmp KERNEL_OFFSET
  ; Hang forever (not really nessecary since we never return from the above)
  jmp $


; Global variables
BOOT_DRIVE db 0
; nasm interprets 13 as carriage return and 10 as line feed
; so this is equivalent to printing '\n message'
; we dont add this for the 32 bit string as we are directly
; accessing video memory rather than using the bios.
MSG_STAGE_2_REAL_MODE db 13,10,"Booted in 16-bit real mode. Stage 2", 0
MSG_PROTECTED_MODE db "Booted into 32-bit protected mode", 0
MSG_LOAD_KERNEL db 13,10,"Loading kernel", 0

; add padding to make sure the second stage takes up exactly one sector
times 512-($-$$) db 0