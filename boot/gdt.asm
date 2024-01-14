; This is the definition of the GDT
; although not very useful for memory protection, we create
; the simplest gdt possible with two sectors (code and data)
; which overlap (basic flat model).
gdt_start:

; This is the null descriptor that we must have for error 
; handling (forgetting to set a segment register).
gdt_null:
  dd 0x0
  dd 0x0

; descriptor for the code segment
gdt_code_kernel:

  ; segment limit low (bits 0 - 15)
  dw 0xffff 

  ; segement base i.e start of memory confusingly this isnt
  ; all stored in consectutive memory addresses
  dw 0x0   

  ; more bits of the base 
  db 0x0   

  ; flags - these are stored as raw binary since the flags are either 1 or 0 e.g 
  ;The order is 
  ; 1st flags : ( present ) ( privilege ) ( descriptor type )
  ; type flags : ( code ) ( conforming ) ( readable ) ( accessed )
  ; 2nd flags : ( granularity ) (32 - bit default ) (64 - bit seg ) ( AVL )
  db 10011010b ; 1st flags , type flags
  db 11001111b ; 2nd flags, Limit high (bits 16 - 19)

  ; final bits of the base
  db 0x0   

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

; The code segment for when running in user mode 
gdt_code_user:

  ; segment limit
  dw 0xffff 

  ; segement base i.e start of memory confusingly this isnt
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


