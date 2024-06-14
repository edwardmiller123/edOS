os-image : boot_stage_1.bin boot_stage_2.bin kernel.bin interrupts.bin
	cat boot_stage_1.bin boot_stage_2.bin kernel.bin interrupts.bin > os-image

# boot_sect.bin : boot/bootloader.asm
# 	nasm -I 'boot' boot/bootloader.asm -f bin -o boot_sect.bin

boot_stage_1.bin : boot/boot_stage_1.asm
	nasm -I 'boot' boot/boot_stage_1.asm -f bin -o boot_stage_1.bin

boot_stage_2.bin : boot/boot_stage_2.asm
	nasm -I 'boot' boot/boot_stage_2.asm -f bin -o boot_stage_2.bin
	
kernel.bin : kernel_entry.o kernel.o usermode.o shell.o isr.o syscall.o screen.o keyboard.o timer.o threads.o tss.o interrupts.o idt.o IO.o helpers.o mem.o stdlib.o
	ld -m elf_i386 -o kernel.bin -Ttext 0x900 kernel_entry.o kernel.o usermode.o shell.o isr.o syscall.o screen.o keyboard.o timer.o threads.o tss.o interrupts.o idt.o IO.o helpers.o mem.o stdlib.o --oformat binary

interrupts.bin : interrupts.o isr.o syscall.o keyboard.o idt.o screen.o timer.o threads.o tss.o IO.o helpers.o mem.o stdlib.o
	ld -m elf_i386 -o interrupts.bin -Ttext 0x900 interrupts.o isr.o syscall.o keyboard.o timer.o threads.o tss.o idt.o screen.o IO.o helpers.o mem.o stdlib.o --oformat binary	

kernel_entry.o : boot/kernel_entry.asm
	nasm boot/kernel_entry.asm -f elf32 -o kernel_entry.o

shell.o : shell/shell.c
	gcc -fno-pie -ffreestanding -m32 -c shell/shell.c -o shell.o 

stdlib.o : stdlib/stdlib.c
	gcc -fno-pie -ffreestanding -m32 -c stdlib/stdlib.c -o stdlib.o 	

interrupts.o : kernel/interrupts/interrupts.asm
	nasm kernel/interrupts/interrupts.asm -f elf32 -o interrupts.o

usermode.o : kernel/usermode.asm
	nasm kernel/usermode.asm -f elf32 -o usermode.o

threads.o : kernel/threads/threads.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/threads/threads.c -o threads.o

mem.o : kernel/mem.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/mem.c -o mem.o

IO.o : kernel/IO.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/IO.c -o IO.o

syscall.o : kernel/drivers/syscall.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/drivers/syscall.c -o syscall.o

isr.o : kernel/interrupts/isr.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/interrupts/isr.c -o isr.o

tss.o : kernel/interrupts/tss.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/interrupts/tss.c -o tss.o

idt.o : kernel/interrupts/idt.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/interrupts/idt.c -o idt.o

timer.o : kernel/drivers/timer.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/drivers/timer.c -o timer.o

helpers.o : kernel/helpers.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/helpers.c -o helpers.o

screen.o : kernel/drivers/screen.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/drivers/screen.c -o screen.o

keyboard.o : kernel/drivers/keyboard.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/drivers/keyboard.c -o keyboard.o

kernel.o : kernel/main.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/main.c -o kernel.o

clean : 
	rm *.bin *.o os-image

run : os-image
	qemu-system-x86_64 -drive format=raw,file=os-image,if=floppy

debug : os-image
	qemu-system-x86_64 -drive format=raw,file=os-image,if=floppy -monitor stdio