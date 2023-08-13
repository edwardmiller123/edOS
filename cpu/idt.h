#ifndef IDT
#define IDT

// An entry in the IDT
struct idtEntry
{
  // has the address of the entry point of a particular interrupt.
  unsigned short isrLow;
  // location of our code/segment (all segments overlap)
  unsigned short kernelSegment;
  unsigned char reserved;
  unsigned char flags;
  unsigned short isrHigh;
};

// A type which contains the size of the idt. Similar to
// how we initialised the GDT_descriptor during boot.
struct idtDescriptor
{
  unsigned short limit;
  unsigned int base;
};

// define the IDT and the IDT descriptor. Again similar to how the GDT is defined but now
// in c.
idtEntry idt[256];
idtDescriptor idtDesc;

// setIDTEntry initialises an entry in the idt.
void setIDTEntry(int n, unsigned int handler)
{
  idt[n].isrLow = (unsigned short)((handler)&0xffff);
  idt[n].kernelSegment = 0x08;
  idt[n].reserved = 0;
  idt[n].flags = 0x8E;
  idt[n].isrHigh = (unsigned short)(((handler) >> 16) & 0xffff);
}

// setIdt is an assembly wrapper that loads the IDT.
void setIdt()
{
  idtDesc.base = (unsigned int)&idt;
  idtDesc.limit = 256 * sizeof(idtEntry) - 1;
  // call the assembly to actually load the idt.
  // nasm assembly would be lidtl 0x(address of the idt descriptor)
  __asm__ __volatile__("lidtl (%0)"
                       :
                       : "r"(&idtDesc));
}

#endif
