os-image : boot_sect.bin kernel.bin
					cat boot_sect.bin kernel.bin > os-image

boot_sect.bin : bootloader.asm
								nasm bootloader.asm -f bin -o boot_sect.bin

kernel.bin : kernel_entry.o kernel.o
						ld -m elf_i386 -o kernel.bin -Ttext 0x1000	\
						kernel_entry.o kernel.o --oformat binary

kernel_entry.o : kernel_entry.asm
								nasm kernel_entry.asm -f elf32 -o kernel_entry.o

kernel.o : kernel.c
					gcc -fno-pie -ffreestanding -m32 -c kernel.c -o kernel.o

clean : 
	rm *.bin *.o os-image