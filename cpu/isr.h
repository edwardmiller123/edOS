#ifndef ISR
#define ISR

#include "idt.h"
#include "../drivers/screen.h"

// The Isr definitions. These are what gets called for each interrupt.

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

// a way of storing the values of assembly registers in c.
struct registers {
  // the data segment register
  unsigned int ds;
  // 16 bit registers we will push onto the stack
  unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
  unsigned int intNumber, errCode;
  // registers automattically pushed onto the stack by the processor
  // e.g code segment register, stack segemtn register etc...
  unsigned int eip, cs, eflags, useresp, ss;
};

// set each entry in the idt individually;
void isr_install() {
    setIDTEntry(0, (unsigned int)isr0);
    setIDTEntry(1, (unsigned int)isr1);
    setIDTEntry(2, (unsigned int)isr2);
    setIDTEntry(3, (unsigned int)isr3);
    setIDTEntry(4, (unsigned int)isr4);
    setIDTEntry(5, (unsigned int)isr5);
    setIDTEntry(6, (unsigned int)isr6);
    setIDTEntry(7, (unsigned int)isr7);
    setIDTEntry(8, (unsigned int)isr8);
    setIDTEntry(9, (unsigned int)isr9);
    setIDTEntry(10, (unsigned int)isr10);
    setIDTEntry(11, (unsigned int)isr11);
    setIDTEntry(12, (unsigned int)isr12);
    setIDTEntry(13, (unsigned int)isr13);
    setIDTEntry(14, (unsigned int)isr14);
    setIDTEntry(15, (unsigned int)isr15);
    setIDTEntry(16, (unsigned int)isr16);
    setIDTEntry(17, (unsigned int)isr17);
    setIDTEntry(18, (unsigned int)isr18);
    setIDTEntry(19, (unsigned int)isr19);
    setIDTEntry(20, (unsigned int)isr20);
    setIDTEntry(21, (unsigned int)isr21);
    setIDTEntry(22, (unsigned int)isr22);
    setIDTEntry(23, (unsigned int)isr23);
    setIDTEntry(24, (unsigned int)isr24);
    setIDTEntry(25, (unsigned int)isr25);
    setIDTEntry(26, (unsigned int)isr26);
    setIDTEntry(27, (unsigned int)isr27);
    setIDTEntry(28, (unsigned int)isr28);
    setIDTEntry(29, (unsigned int)isr29);
    setIDTEntry(30, (unsigned int)isr30);
    setIDTEntry(31, (unsigned int)isr31);

    setIdt(); // Load with ASM
}

// various exception messages for each intterupt.
// just copied from internet as these are standard errors.
char *exceptionMessages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

// isrHandler prints the corresponding message for the given interrupt
void isrHandler(registers reg) {
    print_string("received interrupt");
    print_string("\n");
    print_string(exceptionMessages[reg.intNumber]);
    print_string("\n");
}

#endif