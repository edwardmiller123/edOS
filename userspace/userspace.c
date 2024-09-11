#include "shell.h"
#include "../stdlib/stdlib.h"

void initUserSpace() {
    // start the shell in a new usermode thread
    createThread(&runShell);
}