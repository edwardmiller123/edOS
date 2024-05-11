#ifndef TSS
#define TSS

// initTSS creates a new TSS instance and stores it at the hardcoded
// memory position that the GDT expects
void initTSS();

#endif