# edOS

A small 32-bit OS for x86 architecture. Has a "shell" that runs in user mode and makes system calls to the kernel 
for hardware access i.e to read from the keyboard and to write to the screen. Kind of pointless really but just done as 
a learning exercise and will hopefully be improved upon in the future.

![Picture of the shell](shell.png?raw=true "Optional Title")

### Current Features
- Interrupt driven PS/2 Keyboard, VGA, Timer
- Usermode code and system calls
- Kernel Threads

## Requirements

The Makefile uses GCC, the GNU linker, [NASM](https://www.nasm.us/) for compliling assembly files
and [Qemu](https://www.qemu.org/) as a CPU emulator (others are available).
Can install GCC, Qemu and NASM (for ubuntu) with,

```
sudo apt-get install nasm gcc qemu-system
```

## Running/Compiling

To compile and run simply do,

```
make run
```
or with debugging output,
```
make debug
```
