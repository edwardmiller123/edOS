#include "../consts.h"

// The TSS is used for hardware task switching. We however just use it for
// permission level switches. When an interrupt (iret) occurs while in ring 3
// the CPU uses whatever is in the esp0 field of the TSS as the new stack position.
typedef struct TSS
{
	int prevTss; // The previous TSS - with hardware task switching these form a kind of backward linked list.
	int esp0;	 // The stack pointer to load when changing to kernel mode.
	int ss0;	 // The stack segment to load when changing to kernel mode.
	int esp1;
	int ss1;
	int esp2;
	int ss2;
	int cr3;
	int eip;
	int eflags;
	int eax;
	int ecx;
	int edx;
	int ebx;
	int esp;
	int ebp;
	int esi;
	int edi;
	int es;
	int cs;
	int ss;
	int ds;
	int fs;
	int gs;
	int ldt;
	short trap;
	short iomap_base;
} __attribute__((packed)) TSS;

// initTSS creates a new TSS instance and stores it at the hardcoded
// memory position that the GDT expects since it has already been created
// by the bootloader
void initTSS()
{
	// hard code the position of the TSS in memory
	TSS *newTSS = TSS_POSITION;
	// set the values we need
	newTSS->esp0 = DEFAULT_STACK;
	newTSS->ss0 = KERNEL_DATA_SEG;
	void *tssSeg = TSS_SEG;
	// load the tss to allow for pivilege level changes (and context switches but
	//  we arent using it for that here)
	__asm__ volatile("ltr %%ax" : : "a"(tssSeg));
}

// updateRing0Stack updates the value of esp0 in the TSS to the provided memory
// address
void updateRing0Stack(void *newStack)
{
	TSS *tss = TSS_POSITION;
	tss->esp0 = newStack;
}