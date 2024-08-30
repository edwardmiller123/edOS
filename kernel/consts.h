#ifndef CONSTS
#define CONSTS

// Note: if the kernel gets to big the TSS will be overwritten so may need to move it
#define TSS_POSITION (void *) 0x9000 // at 36.8kb
#define DEFAULT_STACK (void *) 0x900000 // at 9.44mb
#define HEAP_START (void *) 0x200000 //(2.09mb)
#define KERNEL_CODE_SEG 0x08
#define KERNEL_DATA_SEG 0x10
#define USER_CODE_SEG 0x18
#define USER_DATA_SEG 0x20
#define TSS_SEG 0x28
#define NULL (void*) 0

#endif