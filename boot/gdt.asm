; This is the definition of the GDT
; although not very useful for memory protection, we create
; the simplest gdt possible with sectors (code and data)
; that overlap (basic flat model).
; positions in memory of the sectors:
    ; 0x00: Null descriptor
    ; 0x08: Kernel code segment
    ; 0x10: Kernel data segment
    ; 0x18: User code segment
    ; 0x20: User data segment
    ; 0x28: TSS segment

gdt_start:

; This is the null descriptor that we must have for error 
; handling (forgetting to set a segment register).
gdt_null:
  dd 0x0
  dd 0x0

; descriptor for the code segment
gdt_code_kernel:

  ; The segment limit is the physical size in memory
  ; segment limit low (bits 0 - 15)
  dw 0xffff 

  ; segment base - the address where the segment begins i.e in this case the start of memory 
  ; confusingly this isnt all stored in consectutive memory addresses
  dw 0x0  ; bits 0 - 15

  ; more bits of the base 
  db 0x0  ; bits 16 - 23

  ; flags - these are stored as raw binary since the flags are either 1 or 0 e.g 
  ;The order is 
  ; 1st flags : ( present ) ( privilege DPL ) ( descriptor type )
  ; type flags : ( code ) ( conforming ) ( readable ) ( accessed )
  ; 2nd flags : ( granularity ) (32-bit default ) (64 - bit seg ) ( AVL )
  db 10011010b ; 1st flags , type flags
  db 11001111b ; 2nd flags, Limit high (bits 16 - 19)

  ; final bits of the base
  db 0x0  ; bits 24 - 31

; descriptor for the data segment. In this case it overlaps
; exactly with the code segment.
gdt_data_kernel:
  ; same as above apart from the code/data flags
  dw 0xffff 
  dw 0x0   
  db 0x0   
  db 10010010b ; same as code segment but with the first bit of the type flag set to data
  db 11001111b

  ; final bits of the base
  db 0x0   

; The code segment for when running in user mode. These segments 
; still cover the same address space but we need these
; entries to make the priviledge switch.
gdt_code_user:

  ; segment limit
  dw 0xffff 

  ; segment base i.e start of memory confusingly this isnt
  ; all stored in consectutive memory addresses
  dw 0x0   

  ; more bits of the base 
  db 0x0   

  ; flags. This time the privilege level is set to ring 3 i.e 11 rather than 00
  db 11111010b
  db 11001111b

  ; final bits of the base
  db 0x0   


gdt_data_user:
  ; same as above apart from the code/data flags
  dw 0xffff 
  dw 0x0   
  db 0x0   
  ; flags. This time the privilege level is set to ring 3 i.e 11 rather than 00
  db 11110010b
  db 11001111b

  ; final bits of the base
  db 0x0

gdt_tss:
  dw TSS_SIZE ; limit (bits 0 - 15)
  dw TSS ; 

  ; more bits of the base 
  db 0x0   

  ; flags 
  db 0x89
  ; limit high (bits 16 - 19)
  db 0x40

  ; final bits of the base
  db 0x0   

; This label lets the compiler calculate the size of the gdt.
gdt_end:

; GDT descriptor
gdt_descriptor:

  ; calculates the size of the gdt.
  dw gdt_end - gdt_start - 1

  ; Start address of the gdt.
  dd gdt_start

; useful constants defining our segments
KERNEL_CODE_SEG equ gdt_code_kernel - gdt_start
KERNEL_DATA_SEG equ gdt_data_kernel - gdt_start 

; As a work around to some poor OS design, we initialise the TSS later once
; we have booted into the kernel. We therefor hardcode the TSS segment in the GDT
; and actually load it there later. (fingers crossed).
TSS equ 0x100000
TSS_SIZE equ 108

