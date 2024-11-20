# edOS

A small 32-bit OS for x86 architecture. Has a "shell" that runs in user mode and makes system calls to the kernel 
for hardware access i.e to read from the keyboard and to write to the screen. Kind of pointless really but just done as 
a learning exercise and will hopefully be improved upon in the future.

![Picture of the shell](edOS.png?raw=true "Optional Title")

### Current Features
- Interrupt driven PS/2 Keyboard, VGA, Timer
- Usermode "shell" and system calls
- Kernel Threads
- User Threads

## Requirements

The Makefile uses GCC, the GNU linker, [NASM](https://www.nasm.us/) for compliling assembly files
and [Qemu](https://www.qemu.org/) as a CPU emulator (others are available).
Can install GCC, Qemu and NASM (for ubuntu) with,

```
sudo apt-get install nasm gcc qemu-system
```

For compiling on non-linux or x86 platforms youll need to install [docker](https://docs.docker.com/get-started/get-docker/).

## Running/Compiling

### linux/amd64

To compile the bootable image,

```
make os-image
```
To compile and run in the emulator,

```
make run
```
or with debugging output,
```
make debug
```

### Other platforms

After docker is installed for your platform use the following to compile and run edOS
on qemu in the container,
```
DOCKER_DEFAULT_PLATFORM=linux/amd64 docker compose up build-env --build -d
```

