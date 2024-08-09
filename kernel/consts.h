#ifndef CONSTS
#define CONSTS

#define TSS_POSITION (void *) 0x110000 // at 1.11mb
#define DEFAULT_STACK (void *) 0x900000 // at 9.44mb
#define HEAP_START (void *) 0x200000 //(2.09mb)
#define KERNEL_CODE_SEG 0x08
#define KERNEL_DATA_SEG 0x10
#define USER_CODE_SEG 0x18
#define USER_DATA_SEG 0x20
#define TSS_SEG 0x28
#define NULL (void*) 0

#endif