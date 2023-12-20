#include "screen.h"
#include "../cpu/isr.h"
#include "syscall.h"

void syscall(struct registers r) {
    printString("SYSCALL WORKED\n");
}

void initSyscalls() {
    __asm__ volatile("cli");
    registerInterruptHandler(47, syscall);
    __asm__ volatile("sti");
}