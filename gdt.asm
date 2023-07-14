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
gdt_code:

  ; segment limit
  dw 0xffff 

  ; segement base i.e start of memory confusingly this isnt
  ; all stored in consectutive memory addresses
  dw 0x0   

  ; more bits of the base 
  db 0x0   

  ; flags - these are stored are raw binary since the flags are either 1 or 0 e.g 
  ; code = 1,  writable = 1
  db 10011010b 
  db 11001111b

  ; final bits of the base
  db 0x0   

; descriptor for the data segment. In this case it overlaps
; exactly with the code segment.
gdt_data:
  ; same as above apart from the code/data flags
  dw 0xffff 
  dw 0x0   
  db 0x0   
  db 10010010b 
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
  CODE_SEG equ gdt_code - gdt_start
  DATA_SEG equ gdt_data - gdt_start 


