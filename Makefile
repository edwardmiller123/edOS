os-image : boot_sect.bin kernel.bin interrupts.bin
	cat boot_sect.bin kernel.bin interrupts.bin > os-image

boot_sect.bin : boot/bootloader.asm
	nasm -I 'boot' boot/bootloader.asm -f bin -o boot_sect.bin
	
# TODO: figure out how to link isr.c with the kernel
kernel.bin : kernel_entry.o kernel.o
	ld -m elf_i386 -o kernel.bin -Ttext 0x1000 kernel_entry.o kernel.o --oformat binary

interrupts.bin : interrupts.o isr.o 
	ld -m elf_i386 -o interrupts.bin -Ttext 0x1000 interrupts.o isr.o --oformat binary	

kernel_entry.o : boot/kernel_entry.asm
	nasm boot/kernel_entry.asm -f elf32 -o kernel_entry.o 	

interrupts.o : cpu/interrupts.asm
	nasm cpu/interrupts.asm -f elf32 -o interrupts.o

isr.o : cpu/isr.c
	gcc -fno-pie -ffreestanding -m32 -c cpu/isr.c -o isr.o

kernel.o : kernel/kernel.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/kernel.c -o kernel.o

clean : 
	rm *.bin *.o os-image

run : os-image
	qemu-system-x86_64 -drive format=raw,file=os-image,if=floppy