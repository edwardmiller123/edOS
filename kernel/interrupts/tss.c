#include "../consts.h"
#include "../log.h"
#include "../../stdlib/stdlib.h"

// The TSS is used for hardware task switching. We however just use it for
// permission level switches. When an interrupt (iret) occurs while in ring 3
// the CPU uses whatever is in the esp0 field of the TSS as the new stack position.
typedef struct TSS
{
	int prevTss; // The previous TSS - with hardware task switching these form a kind of backward linked list.
	int esp0;	 // The stack pointer to load when changing to kernel mode.
	int ss0;	 // The stack segment to load when changing to kernel mode. (this is actually only 2 bytes. The first 2 are reserved.)
	int esp1;
	int ss1; // (this is actually only 2 bytes)
	int esp2;
	int ss2; // (this is actually only 2 bytes)
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
	int ldtr;
	short t;
	short iopb; // IO port permission bit map
} __attribute__((packed)) TSS;

// initTSS creates a new TSS instance and stores it at the hardcoded
// memory position that the GDT expects since it has already been created
// by the bootloader
void initTSS()
{
	// hard code the position of the TSS in memory
	TSS *newTSS = TSS_POSITION;

	// set the starting values.

	// initial value for esp0 is out of the way so we dont overwrite memory already in use
	// on the first system call. This value is quickly swapped out so doesnt really matter where it is.
	newTSS->esp0 = 0x700000;
	newTSS->ss0 = KERNEL_DATA_SEG;

	// we dont use the IO permission bit map so we just set it to the size of
	// the TSS
	newTSS->iopb = 104;
	void *tssSeg = TSS_SEG;

	// load the tss to allow for pivilege level changes (and context switches but
	// we arent using it for that here)
	__asm__ volatile("ltr %%ax" : : "a"(tssSeg));
	int args[] = {(int)tssSeg};
	kLogf(INFO, "TSS updated. GDT segment loaded. Address: $", args, 1);
}

// updateRing0Stack updates the value of esp0 in the TSS to the provided memory
// address
void updateRing0Stack(void *newStack)
{
	if (newStack == NULL) {
		kLogError("esp0 cant be NULL");
		return;
	}
	TSS *tss = TSS_POSITION;
	tss->esp0 = newStack;
	int args[] = {(int)newStack};
	kLogf(DEBUG, "TSS updated. Esp0 set to $", args, 1);
}