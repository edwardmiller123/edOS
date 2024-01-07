#include "idt.h"
#include "isr.h"
#include "../drivers/screen.h"

intHdlr interruptHandlers[256];

// isrInstall assigns each isr to an entry in the idt.
void isrInstall()
{
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
}

// irqInstall assigns each isr to an entry in the idt.
void irqInstall() {
    // Install the IRQs
    setIDTEntry(32, (unsigned int)irq0);
    setIDTEntry(33, (unsigned int)irq1);
    setIDTEntry(34, (unsigned int)irq2);
    setIDTEntry(35, (unsigned int)irq3);
    setIDTEntry(36, (unsigned int)irq4);
    setIDTEntry(37, (unsigned int)irq5);
    setIDTEntry(38, (unsigned int)irq6);
    setIDTEntry(39, (unsigned int)irq7);
    setIDTEntry(40, (unsigned int)irq8);
    setIDTEntry(41, (unsigned int)irq9);
    setIDTEntry(42, (unsigned int)irq10);
    setIDTEntry(43, (unsigned int)irq11);
    setIDTEntry(44, (unsigned int)irq12);
    setIDTEntry(45, (unsigned int)irq13);
    setIDTEntry(46, (unsigned int)irq14);
    // 47 is being used for our syscalls
    setIDTEntry(47, (unsigned int)irq15);
}

// initPIC initialises the idt and remaps te pic to accept irq's.
void initPIC() {
    
    // In protected mode we need to remap the PIC as IRQ's (hardware interrupts) 0 - 7 overlap with the
    // default cpu exceptions

    port_byte_out(MASTER_PIC_COMMAND, 0x11);
    port_byte_out(SLAVE_PIC_COMMAND, 0x11);
    port_byte_out(MASTER_PIC_DATA, 0x20);
    port_byte_out(SLAVE_PIC_DATA, 0x28);
    port_byte_out(MASTER_PIC_DATA, 0x04);
    port_byte_out(SLAVE_PIC_DATA, 0x02);

    // set the PIC to 8086 mode
    port_byte_out(MASTER_PIC_DATA, 0x01);
    port_byte_out(SLAVE_PIC_DATA, 0x01);

    // Interrupt masking. Here we are masking all interrupts except irq1.
    // ~ flips all the bits i.e 00 -> 11 etc
    port_byte_out(MASTER_PIC_DATA, ~0x2);
    port_byte_out(SLAVE_PIC_DATA, ~0x0);

    isrInstall();
    irqInstall();

    setIdt(); 
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
    // TODO: Figure out whats causing the spam of invalid opcode
    // when interrupts arent masked.
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
    "Reserved"};

// PICsendEOI sends the end of interrupt command for a given interrupt. If the interrupt was handled
// by the slave (irq > 7) then we must send the command to both the master and slave.
void PICsendEOI(unsigned int irq)
{
    if (irq >= 8)
    {
        port_byte_out(SLAVE_PIC_COMMAND, 0x20);
    }
    port_byte_out(MASTER_PIC_COMMAND, 0x20);
}

// isrHandler prints the corresponding message for the given interrupt
void isrHandler(struct registers reg)
{
    kPrintString("Interrupt received\n");
    printInt(reg.intNumber);
    kPrintString("\n");
    kPrintString(exceptionMessages[reg.intNumber]);
    kPrintString("\n");
}

// registerInterruptHandler assigns a given isr (set of registers) to the given position in
// the array of interrupt handlers
void registerInterruptHandler(unsigned char n, intHdlr handler)
{
    interruptHandlers[n] = handler;
}

// irqHandler sends the EOI command for a given interrupt and calls the appropriate handler function
// for the given irq
void irqHandler(struct registers r)
{
    if (interruptHandlers[r.intNumber] != 0)
    {
        intHdlr handler = interruptHandlers[r.intNumber];
        handler(r);
    }
    PICsendEOI(r.intNumber);
}

int syscallHandler(struct registers r) {
    int output;
    if (interruptHandlers[r.intNumber] != 0)
    {
        intHdlr handler = interruptHandlers[r.intNumber];
        output = handler(r);
    }
    PICsendEOI(r.intNumber);
    return output;
}
