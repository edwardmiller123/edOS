#include "screen.h"
#include "../cpu/isr.h"
#include "keyboard.h"
#include "syscall.h"

// syscall reads the provided register values and handles the system call accordingly
int syscall(struct registers r)
{
    char *input = r.eax;
    int driverCode = r.ebx;
    int functionCode = r.ecx;
    char *output = 0;
    switch (driverCode)
    {
    case 1:
        if (functionCode == 2)
        {
            kPrintString(input);
        }
        break;
    case 2:
        if (functionCode == 1)
        {
            output = readKeyBuffer();
        }
    }
    return output;
}

// initSyscalls registers the syscall handler function in the IDT
void initSyscalls()
{
    __asm__ volatile("cli");
    registerInterruptHandler(47, syscall);
    __asm__ volatile("sti");
}