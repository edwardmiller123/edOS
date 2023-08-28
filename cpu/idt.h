#ifndef IDT
#define IDT

// An entry in the IDT
struct idtEntry
{
  // has the address of the entry point of a particular interrupt.
  unsigned short isrLow;
  // location of our code/data segment (all segments overlap)
  unsigned short kernelSegment;
  unsigned char reserved;
  unsigned char flags;
  unsigned short isrHigh;
};

// A type which contains the size of the idt. Similar to
// how we initialised the GDT descriptor during boot.
struct idtDescriptor
{
  unsigned short limit;
  unsigned int base;
};

// setIDTEntry initialises an entry in the idt.
void setIDTEntry(int n, unsigned int handler);

// setIdt is an assembly wrapper that loads the IDT.
void setIdt();

#endif
