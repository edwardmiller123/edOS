# edOS

A small 32-bit OS for x86 architecture. More of a type writer really but a long term
side project which will hopefully be improved on.

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