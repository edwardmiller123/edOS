#ifndef MEM
#define MEM

// kMalloc allocates memory on the heap and returns a pointer. 
// Memory is allocated in blocks of equal size (24 bytes) in a
// structure where the 1st 4 bytes
// indicate whether the block is in use or not and the 2nd 4 bytes denotes how many blocks 
// are used contigously i.e if the size to be allocated is larger than a single block then two are allocated.
// kMalloc will first iterate through allocated blocks to check if any are free and large enough before
// creating a new block.
void * kMalloc(int size);

// kFree frees the heap memory corresponding to the given pointer.
// It checks if the block is allocated and if so sets the "taken" 4 bytes to 0 i.e free
void kFree(void * ptr);

// setAtAdress stores the given value at the provided bare metal memory address.
// Allows us to store values at specific addresses in C (Normally the compiler wouldnt allow this).
// Caution: This uses eax and ebx to actually do the move so any values already there
// wil be overwritten which could lead to undefined behviour.
void setAtAddress(int val, void * address);

// getAtAddress reads the value stored at a bare metal memory address.
// Allows us to read values at specific addresses in C (Normally the compiler wouldnt allow this).
// Caution: This uses eax and ebx to actually do the move so any values already there
// wil be overwritten which could lead to undefined behviour.
void * getAtAddress(void * address);

#endif