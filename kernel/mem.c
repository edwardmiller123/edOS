#include "mem.h"
#include "drivers/screen.h"

#define HEAP_START 0x100000 //(1mb)
#define STD_BLOCK_SIZE 24
#define USEABLE_BLOCK_SIZE 16

static void *heapTop = HEAP_START;

// kMalloc allocates memory on the heap and returns a pointer.
// Memory is allocated in "standard blocks" of equal size (24 bytes) in a
// structure where the 1st 4 bytes indicate whether the block is in use or
// not (maybe make this less wasteful in future) and the 2nd 4 bytes denotes how many blocks are used contigously i.e if the
// size to be allocated is larger than a single block then an appropriate number are allocated.
// kMalloc will first iterate through allocated blocks to check if any are free and large enough before
// creating a new block.
void *kMalloc(int size)
{
  void *currentBlock = HEAP_START;
  int freeBlockFound = 0;
  int allocatedStdBlockCount = 1;
  // checking first 4 bytes to see if each block is free
  while (*(int *)currentBlock != 0)
  {
    // check if the current block consists of multiple std blocks, in which
    // case we need to skip the extra blocks in use
    // for future me: here we increment the current block by the standard block size times the number of
    // standard blocks in use which we store in the 5th byte.
    currentBlock += STD_BLOCK_SIZE * (*(int *)(currentBlock + 4));
  }
  // TODO: Check we dont exceed max available memory
  // set the block as used
  *(int *)currentBlock = 1;

  // calculate how many std blocks to allocate
  if (size > USEABLE_BLOCK_SIZE)
  {
    while (size > USEABLE_BLOCK_SIZE * allocatedStdBlockCount)
    {
      allocatedStdBlockCount += 1;
    }
  }

  *(int *)(currentBlock + 4) = allocatedStdBlockCount;

  void *useableMemory = currentBlock + 8;
  return useableMemory;
}

// kFree frees the heap memory corresponding to the given pointer.
// It  checks if the block is allocated and if so sets the "taken" 4 bytes to 0 i.e free
void kFree(void *ptr)
{
  int blockUsed = *(int *)(ptr - 8);
  if (blockUsed == 0)
  {
    return;
  }
  *(int *)(ptr - 8) = 0;
}

// setAtAdress stores the given value at the provided bare metal memory address.
// Allows us to store values at specific addresses in C (Normally the compiler wouldnt allow this).
// Caution: This uses eax and ebx to actually do the move so any values already there
// wil be overwritten which could lead to undefined behviour.
void setAtAddress(int val, void * address) {
  __asm__ volatile("movl %%eax, (%%ebx)" : : "a"(val), "b"(address));
}

// getAtAddress reads the value stored at a bare metal memory address.
// Allows us to read values at specific addresses in C (Normally the compiler wouldnt allow this).
// Caution: This uses eax and ebx to actually do the move so any values already there
// wil be overwritten which could lead to undefined behviour.
void * getAtAddress(void * address) {
  void * val;
  __asm__ volatile("movl (%%ebx), %%eax" : "=a"(val): "b"(address));
  return val;
}