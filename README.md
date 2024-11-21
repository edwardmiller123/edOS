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

For compiling on other platforms (e.g arm64) the easiest way is to build the image in a
docker container then copy it over to the host.
You can create the container and compile the image with,
```
DOCKER_DEFAULT_PLATFORM=linux/amd64 docker compose up build-env --build -d
```
The image can then be copied out of the container to your current directory with the following command
```
docker cp <containerId>:/edOS/os-image .
```
Assuming you have qemu installed you then run edOS in the emulator with,
```
qemu-system-x86_64 -drive format=raw,file=os-image,if=floppy -monitor stdio
```
