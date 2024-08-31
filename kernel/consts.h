#ifndef CONSTS
#define CONSTS

// Various constants used throughout the kernel

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

//////////////////////////////////////////////////////
////////////////// Memory layout /////////////////////
//////////////////////////////////////////////////////


//    .
//    .
// 0x901800 - kernel thread stack (threads created aove here in increments of 0x1800)
// 0x900000 - kernel stack

// 0x800000 - initial kernel stack/user stacks (grows downwards)
// set the inital stack here away from our future kernel stack so they dont conflict

// 0x200000 - heap memory (grows upwards)

// 0x9000 - TSS

// 0x7C00 - stage 1 bootsector loaded here

// 0x900 - kernel begins. About 30kb in size currently

// 0x600 - stage 2 boot sector loaded here

// 0x0
///////////////////////////////////////////////////////