#ifndef ISR
#define ISR

#include "../threads.h"

#define MASTER_PIC_COMMAND 0x20
#define MASTER_PIC_DATA 0x21
#define SLAVE_PIC_COMMAND 0xA0
#define SLAVE_PIC_DATA 0xA1
#define KERNEL_MODE_FLAGS 0x8E
#define USER_MODE_FLAGS 0xEE

// wrapper for sti instruction (enables interrupts)
void sti();

// wrapper for cli instruction (disables interrupts)
void cli();

// hlt is a C wrapper for the hlt instruction
void hlt();

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

// hardware interrupt definitions (IRQ's)
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void isrInstall();

void irqInstall();

void initPIC();

void PICsendEOI(unsigned int irq);

// isrHandler prints the corresponding message for the given interrupt and halts.
// We halt as most errors at this stage are unrecoverable so no point spaming error messages.
void isrHandler(struct registers reg);

// intHndlr is a pointer to an interrupt handler function that takes a registers and returns an int.
typedef int (*intHdlr)(struct registers);

void registerInterruptHandler(unsigned char n, intHdlr handler);

void irqHandler(struct registers r);

#endif