#ifndef SYSCALL
#define SYSCALL

int syscall(struct registers r);

void initSyscalls();

#endif