#include "screen.h"
#include "../cpu/isr.h"
#include "keyboard.h"
#include "syscall.h"

void syscall(struct registers r)
{
    // kPrintString("syscall received\n");
    // printInt(r.ebx);
    // kPrintString("\n");
    // kPrintString(r.eax);
    // kPrintString("\n");
    // printInt(r.ecx);
    // kPrintString("\n");
    char *input = r.eax;
    int driverCode = r.ebx;
    int functionCode = r.ecx;
    switch (driverCode)
    {
    case 1:
        if (functionCode == 2)
        {
            kPrintString(input);
        }
        break;
    case 2:
        if (functionCode == 1) {
            char * output = readKeyBuffer();
            __asm__ volatile("movl %0, %%eax" : : "a"(output));
        }

    }
}

void initSyscalls()
{
    __asm__ volatile("cli");
    registerInterruptHandler(47, syscall);
    __asm__ volatile("sti");
}