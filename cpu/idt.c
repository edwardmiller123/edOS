#include "idt.h"

// define the IDT and the IDT descriptor. Again similar to how the GDT is defined but now
// in c.
struct idtEntry idt[256];
struct idtDescriptor idtDesc;

// setIDTEntry initialises an entry in the idt.
void setIDTEntry(int n, unsigned int handler)
{
  idt[n].isrLow = (unsigned short)((handler)&0xffff);
  idt[n].kernelSegment = 0x10;
  idt[n].reserved = 0;
  idt[n].flags = 0x8E;
  idt[n].isrHigh = (unsigned short)(((handler) >> 16) & 0xffff);
}

// setIdt is an assembly wrapper that loads the IDT.
void setIdt()
{
  idtDesc.base = (unsigned int)&idt;
  idtDesc.limit = 256 * sizeof(struct idtEntry) - 1;
  // call the assembly to actually load the idt.
  // nasm assembly would be lidtl (address of the idt descriptor)
  __asm__ volatile("lidtl (%0)"
                       :
                       : "r"(&idtDesc));
  // enable interrupts again
  // __asm__ volatile ("sti");
}

