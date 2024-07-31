#ifndef TSS
#define TSS

// initTSS creates a new TSS instance and stores it at the hardcoded
// memory position that the GDT expects
void initTSS();

// updateRing0Stack updates the value of esp0 in the TSS to the provided memory
// address
void updateRing0Stack(void * newStack);

#endif