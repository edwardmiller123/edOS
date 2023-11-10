# edOS

A small 32-bit OS for an x86 chip. More of a type writer really but a long term
side project which will hopefully be improved on.

## Requirements

The Makefile uses GCC, the GNU linker, [NASM](https://www.nasm.us/) for compliling the assembly files
and [Qemu](https://www.qemu.org/) as a CPU emulator (others are available).
If on linux everything should just work.
Can install Qemu and NASM (for ubuntu) with,

```
sudo apt-get install nasm qemu-system
```

## Running/Compiling

To compile and run simply do,

```
make run
```