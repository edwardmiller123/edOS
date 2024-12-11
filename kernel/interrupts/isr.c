#include "idt.h"
#include "isr.h"
#include "../log.h"
#include "../../stdlib/stdlib.h"
#include "../io.h"
#include "../drivers/screen.h"

intHdlr interruptHandlers[256];

// wrapper for sti instruction (enables interrupts)
void sti()
{
    __asm__ volatile("sti");
    kLogDebug("Interrupts enabled");
}

// wrapper for cli instruction (disables interrupts)
void cli()
{
    __asm__ volatile("cli");
    kLogDebug("Interrupts disabled");
}

// hlt is a C wrapper for the hlt instruction
void hlt()
{
    __asm__ volatile("hlt");
}

// isrInstall assigns each isr to an entry in the idt.
void isrInstall()
{
    setIDTEntry(0, (unsigned int)isr0, KERNEL_MODE_FLAGS);
    setIDTEntry(1, (unsigned int)isr1, KERNEL_MODE_FLAGS);
    setIDTEntry(2, (unsigned int)isr2, KERNEL_MODE_FLAGS);
    setIDTEntry(3, (unsigned int)isr3, KERNEL_MODE_FLAGS);
    setIDTEntry(4, (unsigned int)isr4, KERNEL_MODE_FLAGS);
    setIDTEntry(5, (unsigned int)isr5, KERNEL_MODE_FLAGS);
    setIDTEntry(6, (unsigned int)isr6, KERNEL_MODE_FLAGS);
    setIDTEntry(7, (unsigned int)isr7, KERNEL_MODE_FLAGS);
    setIDTEntry(8, (unsigned int)isr8, KERNEL_MODE_FLAGS);
    setIDTEntry(9, (unsigned int)isr9, KERNEL_MODE_FLAGS);
    setIDTEntry(10, (unsigned int)isr10, KERNEL_MODE_FLAGS);
    setIDTEntry(11, (unsigned int)isr11, KERNEL_MODE_FLAGS);
    setIDTEntry(12, (unsigned int)isr12, KERNEL_MODE_FLAGS);
    setIDTEntry(13, (unsigned int)isr13, KERNEL_MODE_FLAGS);
    setIDTEntry(14, (unsigned int)isr14, KERNEL_MODE_FLAGS);
    setIDTEntry(15, (unsigned int)isr15, KERNEL_MODE_FLAGS);
    setIDTEntry(16, (unsigned int)isr16, KERNEL_MODE_FLAGS);
    setIDTEntry(17, (unsigned int)isr17, KERNEL_MODE_FLAGS);
    setIDTEntry(18, (unsigned int)isr18, KERNEL_MODE_FLAGS);
    setIDTEntry(19, (unsigned int)isr19, KERNEL_MODE_FLAGS);
    setIDTEntry(20, (unsigned int)isr20, KERNEL_MODE_FLAGS);
    setIDTEntry(21, (unsigned int)isr21, KERNEL_MODE_FLAGS);
    setIDTEntry(22, (unsigned int)isr22, KERNEL_MODE_FLAGS);
    setIDTEntry(23, (unsigned int)isr23, KERNEL_MODE_FLAGS);
    setIDTEntry(24, (unsigned int)isr24, KERNEL_MODE_FLAGS);
    setIDTEntry(25, (unsigned int)isr25, KERNEL_MODE_FLAGS);
    setIDTEntry(26, (unsigned int)isr26, KERNEL_MODE_FLAGS);
    setIDTEntry(27, (unsigned int)isr27, KERNEL_MODE_FLAGS);
    setIDTEntry(28, (unsigned int)isr28, KERNEL_MODE_FLAGS);
    setIDTEntry(29, (unsigned int)isr29, KERNEL_MODE_FLAGS);
    setIDTEntry(30, (unsigned int)isr30, KERNEL_MODE_FLAGS);
    setIDTEntry(31, (unsigned int)isr31, KERNEL_MODE_FLAGS);
}

// irqInstall assigns each isr to an entry in the idt.
void irqInstall()
{
    // Install the IRQs

    setIDTEntry(32, (unsigned int)irq0, KERNEL_MODE_FLAGS); // timer
    setIDTEntry(33, (unsigned int)irq1, KERNEL_MODE_FLAGS); // keyboard
    setIDTEntry(34, (unsigned int)irq2, KERNEL_MODE_FLAGS);
    setIDTEntry(35, (unsigned int)irq3, KERNEL_MODE_FLAGS);
    setIDTEntry(36, (unsigned int)irq4, KERNEL_MODE_FLAGS);
    setIDTEntry(37, (unsigned int)irq5, KERNEL_MODE_FLAGS);
    setIDTEntry(38, (unsigned int)irq6, KERNEL_MODE_FLAGS);
    setIDTEntry(39, (unsigned int)irq7, KERNEL_MODE_FLAGS);
    setIDTEntry(40, (unsigned int)irq8, KERNEL_MODE_FLAGS);
    setIDTEntry(41, (unsigned int)irq9, KERNEL_MODE_FLAGS);
    setIDTEntry(42, (unsigned int)irq10, KERNEL_MODE_FLAGS);
    setIDTEntry(43, (unsigned int)irq11, KERNEL_MODE_FLAGS);
    setIDTEntry(44, (unsigned int)irq12, KERNEL_MODE_FLAGS);
    setIDTEntry(45, (unsigned int)irq13, KERNEL_MODE_FLAGS);
    setIDTEntry(46, (unsigned int)irq14, KERNEL_MODE_FLAGS);
    setIDTEntry(47, (unsigned int)irq15, USER_MODE_FLAGS); // syscalls
}

// initPIC initialises the idt and remaps te pic to accept irq's.
void initPIC()
{
    // In protected mode we need to remap the PIC as IRQ's (hardware interrupts) 0 - 7 overlap with the
    // default cpu exceptions

    writeByte(MASTER_PIC_COMMAND, 0x11);
    writeByte(SLAVE_PIC_COMMAND, 0x11);
    writeByte(MASTER_PIC_DATA, 0x20);
    writeByte(SLAVE_PIC_DATA, 0x28);
    writeByte(MASTER_PIC_DATA, 0x04);
    writeByte(SLAVE_PIC_DATA, 0x02);

    // set the PIC to 8086 mode
    writeByte(MASTER_PIC_DATA, 0x01);
    writeByte(SLAVE_PIC_DATA, 0x01);

    // Interrupt masking
    // ~ flips all the bits i.e 00 -> 11 etc
    // mask all but the keyboard and timer
    writeByte(MASTER_PIC_DATA, 0xFC);
    // mask all irqs on the slave except the syscalls
    writeByte(SLAVE_PIC_DATA, ~0x0);

    isrInstall();
    irqInstall();

    setIdt();

    kLogInfo("PIC initialised");
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
    "Reserved"};

// PICsendEOI sends the end of interrupt command for a given interrupt. If the interrupt was handled
// by the slave (irq > 7) then we must send the command to both the master and slave.
void PICsendEOI(unsigned int irq)
{
    if (irq >= 8)
    {
        writeByte(SLAVE_PIC_COMMAND, 0x20);
    }
    writeByte(MASTER_PIC_COMMAND, 0x20);
}

// isrHandler prints the corresponding message for the given interrupt and halts.
// We halt as most errors at this stage are unrecoverable so no point spaming error messages.
void isrHandler(struct registers reg)
{
    char newStr[30];
    char * msg = strConcat(
        "Kernel Interrupted\n ISR: $, Error Code: $\n Reason: ", 
        exceptionMessages[reg.intNumber], 
        newStr
    );
    int args[] = {reg.intNumber, reg.errCode};
    kLogf(FATAL, msg, args, 2);
}

// registerInterruptHandler assigns a given isr handler to the given position in
// the array of interrupt handlers
void registerInterruptHandler(unsigned char n, intHdlr handler)
{
    interruptHandlers[n] = handler;
}

// irqHandler sends the EOI command for a given interrupt and calls the appropriate handler function
// for the given irq.
void irqHandler(struct registers r)
{
    int arg[] = {r.intNumber};
    kLogf(DEBUG, "Kernel Interrupted, IRQ: $", arg, 1);

    if (interruptHandlers[r.intNumber] != 0)
    {
        intHdlr handler = interruptHandlers[r.intNumber];
        handler(r);
    }

    // We only load a different thread for timer interrupts.
    if (r.intNumber == 32)
    {
        threadSwitch(r);
    }
    PICsendEOI(r.intNumber);
}