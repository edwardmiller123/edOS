#ifndef SYSCALL
#define SYSCALL

// syscall reads the provided register values and handles the system call accordingly
int syscall(struct registers r);

void initSyscalls();

#endif