os-image : boot_sect.bin kernel.bin interrupts.bin
	cat boot_sect.bin kernel.bin interrupts.bin > os-image

boot_sect.bin : boot/bootloader.asm
	nasm -I 'boot' boot/bootloader.asm -f bin -o boot_sect.bin
	
kernel.bin : kernel_entry.o kernel.o usermode.o shell.o isr.o syscall.o screen.o keyboard.o timer.o interrupts.o idt.o IO.o mem.o stdlib.o
	ld -m elf_i386 -o kernel.bin -Ttext 0x1000 kernel_entry.o kernel.o usermode.o shell.o isr.o syscall.o screen.o keyboard.o timer.o interrupts.o idt.o IO.o mem.o stdlib.o --oformat binary

interrupts.bin : interrupts.o isr.o syscall.o keyboard.o idt.o screen.o IO.o mem.o stdlib.o
	ld -m elf_i386 -o interrupts.bin -Ttext 0x1000 interrupts.o isr.o syscall.o keyboard.o idt.o screen.o IO.o mem.o stdlib.o --oformat binary	

kernel_entry.o : boot/kernel_entry.asm
	nasm boot/kernel_entry.asm -f elf32 -o kernel_entry.o

shell.o : shell/shell.c
	gcc -g -fno-pie -ffreestanding -m32 -c shell/shell.c -o shell.o 

stdlib.o : stdlib/stdlib.c
	gcc -g -fno-pie -ffreestanding -m32 -c stdlib/stdlib.c -o stdlib.o 	

interrupts.o : interrupts/interrupts.asm
	nasm interrupts/interrupts.asm -f elf32 -o interrupts.o

usermode.o : kernel/usermode.asm
	nasm kernel/usermode.asm -f elf32 -o usermode.o

mem.o : kernel/mem.c
	gcc -g -fno-pie -ffreestanding -m32 -c kernel/mem.c -o mem.o

IO.o : kernel/IO.c
	gcc -g -fno-pie -ffreestanding -m32 -c kernel/IO.c -o IO.o

syscall.o : drivers/syscall.c
	gcc -g -fno-pie -ffreestanding -m32 -c drivers/syscall.c -o syscall.o

isr.o : interrupts/isr.c
	gcc -g -fno-pie -ffreestanding -m32 -c interrupts/isr.c -o isr.o

idt.o : interrupts/idt.c
	gcc -g -fno-pie -ffreestanding -m32 -c interrupts/idt.c -o idt.o

timer.o : drivers/timer.c
	gcc -g -fno-pie -ffreestanding -m32 -c drivers/timer.c -o timer.o

screen.o : drivers/screen.c
	gcc -g -fno-pie -ffreestanding -m32 -c drivers/screen.c -o screen.o

keyboard.o : drivers/keyboard.c
	gcc -g -fno-pie -ffreestanding -m32 -c drivers/keyboard.c -o keyboard.o

kernel.o : kernel/kernel.c
	gcc -g -fno-pie -ffreestanding -m32 -c kernel/kernel.c -o kernel.o

clean : 
	rm *.bin *.o os-image

run : os-image
	qemu-system-x86_64 -drive format=raw,file=os-image,if=floppy

debug : os-image
	qemu-system-x86_64 -drive format=raw,file=os-image,if=floppy -monitor stdio