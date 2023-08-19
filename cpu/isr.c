#include "idt.h"
#include "isr.h"
#include "../drivers/screen.h"

// set each entry in the idt individually;
void isrInstall() {
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

// various exception messages for each interrupt.
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
// TODO: why cant it find printString
void isrHandler(struct registers reg) {
    printString("received interrupt");
    printString("\n");
    printString(exceptionMessages[reg.intNumber]);
    printString("\n");
}