#include "screen.h"
#include "../cpu/isr.h"
#include "keyboard.h"
#include "syscall.h"

int syscall(struct registers r)
{
    int test = 9;
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
        if (functionCode == 1)
        {
            char *output = readKeyBuffer();
            __asm__ volatile("movl %0, %%esi" : : "r"(test));

            int outputDebug;
            __asm__ volatile("movl %%esi, %0" : "=r"(outputDebug) :);
            kPrintString("Value in register during call: ");
            printInt(outputDebug);
            kPrintString("\n");
        }
    }
    return test;
}

void initSyscalls()
{
    __asm__ volatile("cli");
    registerInterruptHandler(47, syscall);
    __asm__ volatile("sti");
}