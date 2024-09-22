#ifndef TYPES
#define TYPES

// a way of storing the values of the registers when 
// calling the interrupt handler from assembly.
struct registers {
  // just before the call instruction we push esp on the stack to get its value.
  unsigned int stubesp;
  // the data segment register
  unsigned int ds;
  // general purpose registers we will push onto the stack
  unsigned int edi;
  unsigned int esi;
  unsigned int ebp;
  unsigned int unused; // this is always empty as the pusha instruction skips esp
  unsigned int ebx;
  unsigned int edx;
  unsigned int ecx;
  unsigned int eax;
  unsigned int intNumber;
  unsigned int errCode;

  // registers automatically pushed onto the stack by the processor
  // These are what iret expects to be on the stack
  unsigned int eip;
  unsigned int cs;
  unsigned int eflags;
  // useresp and ss are only pushed if a priviledge change occurs
  unsigned int useresp; 
  unsigned int ss;
}__attribute__((packed));

#endif

