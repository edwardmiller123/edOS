#ifndef SYSCALL
#define SYSCALL

// syscall reads the provided register values and handles the system call accordingly
int syscallHandler(struct registers r);

#endif