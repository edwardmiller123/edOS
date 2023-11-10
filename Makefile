os-image : boot_sect.bin kernel.bin interrupts.bin
	cat boot_sect.bin kernel.bin interrupts.bin > os-image

boot_sect.bin : boot/bootloader.asm
	nasm -I 'boot' boot/bootloader.asm -f bin -o boot_sect.bin
	
kernel.bin : kernel_entry.o kernel.o shell.o isr.o screen.o keyboard.o interrupts.o idt.o I_O_asm_helpers.o utils.o
	ld -m elf_i386 -o kernel.bin -Ttext 0x1000 kernel_entry.o kernel.o shell.o isr.o screen.o keyboard.o interrupts.o idt.o I_O_asm_helpers.o utils.o --oformat binary

interrupts.bin : interrupts.o isr.o idt.o screen.o I_O_asm_helpers.o utils.o
	ld -m elf_i386 -o interrupts.bin -Ttext 0x1000 interrupts.o isr.o idt.o screen.o I_O_asm_helpers.o utils.o --oformat binary	

kernel_entry.o : boot/kernel_entry.asm
	nasm boot/kernel_entry.asm -f elf32 -o kernel_entry.o

shell.o : shell/shell.c
	gcc -fno-pie -ffreestanding -m32 -c shell/shell.c -o shell.o 	

interrupts.o : cpu/interrupts.asm
	nasm cpu/interrupts.asm -f elf32 -o interrupts.o

utils.o : kernel/utils.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/utils.c -o utils.o

I_O_asm_helpers.o : kernel/I_O_asm_helpers.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/I_O_asm_helpers.c -o I_O_asm_helpers.o

isr.o : cpu/isr.c
	gcc -fno-pie -ffreestanding -m32 -c cpu/isr.c -o isr.o

idt.o : cpu/idt.c
	gcc -fno-pie -ffreestanding -m32 -c cpu/idt.c -o idt.o

screen.o : drivers/screen.c
	gcc -fno-pie -ffreestanding -m32 -c drivers/screen.c -o screen.o

keyboard.o : drivers/keyboard.c
	gcc -fno-pie -ffreestanding -m32 -c drivers/keyboard.c -o keyboard.o

kernel.o : kernel/kernel.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/kernel.c -o kernel.o

clean : 
	rm *.bin *.o os-image

run : os-image
	qemu-system-x86_64 -drive format=raw,file=os-image,if=floppy