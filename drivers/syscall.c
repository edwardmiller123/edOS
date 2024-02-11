#include "screen.h"
#include "../interrupts//isr.h"
#include "keyboard.h"
#include "syscall.h"

// syscallHandler reads the provided register values and handles the system call accordingly
// code (DC) and function code (FC).
// screen: DC = 1
// keyboard: DC = 2
// read: FC = 1
// write: FC = 2
int syscallHandler(struct registers r)
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
    PICsendEOI(r.intNumber);
    return output;
}