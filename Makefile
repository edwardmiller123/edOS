os-image : boot_stage_1.bin boot_stage_2.bin kernel.bin
	cat boot_stage_1.bin boot_stage_2.bin kernel.bin > os-image

boot_stage_1.bin : boot/boot_stage_1.asm
	nasm -I 'boot' boot/boot_stage_1.asm -f bin -o boot_stage_1.bin

boot_stage_2.bin : boot/boot_stage_2.asm
	nasm -I 'boot' boot/boot_stage_2.asm -f bin -o boot_stage_2.bin
	
kernel.bin : kernel_entry.o kernel.o enterusermode.o userspace.o shell.o isr.o syscall.o log.o screen.o keyboard.o timer.o threads.o tss.o interrupts.o idt.o IO.o mem.o stdlib.o
	ld -m elf_i386 -o kernel.bin -Ttext 0x900 kernel_entry.o kernel.o enterusermode.o userspace.o shell.o isr.o syscall.o log.o screen.o keyboard.o timer.o threads.o tss.o interrupts.o idt.o IO.o mem.o stdlib.o --oformat binary

kernel_entry.o : boot/kernel_entry.asm
	nasm boot/kernel_entry.asm -f elf32 -o kernel_entry.o

userspace.o : userspace/userspace.c
	gcc -fno-pie -ffreestanding -m32 -c userspace/userspace.c -o userspace.o

shell.o : userspace/shell.c
	gcc -fno-pie -ffreestanding -m32 -c userspace/shell.c -o shell.o 

stdlib.o : stdlib/stdlib.c
	gcc -fno-pie -ffreestanding -m32 -c stdlib/stdlib.c -o stdlib.o 	

interrupts.o : kernel/interrupts/interrupts.asm
	nasm kernel/interrupts/interrupts.asm -f elf32 -o interrupts.o

enterusermode.o : kernel/enterusermode.asm
	nasm kernel/enterusermode.asm -f elf32 -o enterusermode.o

log.o : kernel/log.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/log.c -o log.o

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

screen.o : kernel/drivers/screen.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/drivers/screen.c -o screen.o

keyboard.o : kernel/drivers/keyboard.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/drivers/keyboard.c -o keyboard.o

kernel.o : kernel/main.c
	gcc -fno-pie -ffreestanding -m32 -c kernel/main.c -o kernel.o

clean : 
	rm *.bin *.o os-image

run : os-image
	qemu-system-x86_64 -drive format=raw,file=os-image,if=floppy -monitor stdio 

debug : os-image
	qemu-system-x86_64 -drive format=raw,file=os-image,if=floppy -monitor stdio -d int -no-reboot