[bits 16]
; We will use dh as our function argument containing the disk sectors to be read.
; We will read n sectors from a (virtual) floppy disk.
disk_load:
  ; store dx on the stack
  push dx

  ; just like printing, this signals to the bios that we wish to use
  ; the disk read interrupt.
  mov ah, 0x02

  ; store dh in al
  mov al, dh

  ; Select the cylinder and head to do the read
  mov ch, 0x00
  mov dh, 0x00

  ; start reading from second sector as the first contains our bootsector
  ; which has already been loaded.
  mov cl, 0x02

  ; trigger the interrupt to do the read
  int 0x13

  ; check for disk read errors. There is a special flags register in which
  ; the carry flag gets set to general fault by the bios if the disk read fails.
  ; here we jump if the carry flag has been set.
  jc disk_error_general_fault
  
  ; restore dx from the stack
  pop dx

  ; check that the number of sectors actually read (stored in al by the bios) is equal
  ; to the number we expected to read (from our function arg dh).
  cmp dh, al
  jne disk_error_sector_mismatch
  ret

disk_error_sector_mismatch:
  ; use our print function to display an error and then hang.
  mov bx, DISK_ERROR_SECTOR_MISMATCH
  call print_string
  jmp $

DISK_ERROR_SECTOR_MISMATCH db 13,10,"Wrong number of sectors read", 0

disk_error_general_fault:
  ; use our print function to display an error and then hang.
  mov bx, DISK_ERROR_GENERAL_FAULT
  call print_string
  jmp $

DISK_ERROR_GENERAL_FAULT db 13,10,"Error reading disk GENERAL FAULT", 0


