; indicate to the cpu that the following instructions are 16 bit
[bits 16]

; switch to 32 bit mode
switch_to_pm:

  ; dissable interrupts as they are 16 bit bios instructions which will
  ; not be valid in 32 bit mode.
  cli

  ; load the gdt descriptor which contains the start address and sixe of the gdt
  lgdt [gdt_descriptor]

  ; To actually make the switch we change the first bit 
  ; of a special control register cr0. We cant do this directly so 
  ; we make the change on a general purpose register first.

  ; store cr0 in eax
  mov eax, cr0

  ; use the or command to only change 1 bit of eax
  or eax, 0x1

  ; set the value of cr0 back to the edited eax
  mov cr0, eax

  ; make a far jump to a new segment where are 32 bit code is.
  ; also forces cpu to clear cache of pre-fetched instructions.
  jmp KERNEL_CODE_SEG:init_pm

; indicate that the following instructions are in 32 bit
[bits 32]

; once in 32 bit we need to set all the old 16 bit segemnt registers to
; the addresses of our new segments defined in the gdt.
init_pm:

  ; store the new segment in ax inorder to set all the old 16 bit segment registers
  ; to KERNEL_DATA_SEG
  mov ax, KERNEL_DATA_SEG
  mov ds, ax
  mov ss, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  ; update the stack position so its well into free space to avoid 
  ; over writing things.
  mov ebp, 0x800000 ; (9.44mb)
  mov esp, ebp

  ; call a label that is comfortably in our 32 bit section. Since we never return from here
  ; this also just be a jmp.
  call BEGIN_PM