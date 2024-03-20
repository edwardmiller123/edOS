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
} __attribute__((packed));

// A type which contains the size of the idt. Similar to
// how we initialised the GDT descriptor during boot.
struct idtDescriptor
{
  unsigned short limit;
  unsigned int base;
} __attribute__((packed));

// setIDTEntry initialises an entry in the idt. Assigns a handler
// address to an interrupt number as well as flags for specifiying 
// the privilige level.
void setIDTEntry(int n, unsigned int handler, int flags);

// setIdt is an assembly wrapper that loads the IDT.
void setIdt();

#endif
