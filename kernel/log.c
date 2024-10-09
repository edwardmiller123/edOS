#include "log.h"
#include "drivers/screen.h"
#include "consts.h"
#include "../stdlib/stdlib.h"
static LogLevel level = INFO;

// setLogLevel sets the global logging level
void setLogLevel(LogLevel logLevel) {
    level = logLevel;
}


void log(char * levelStr, char* msg, char attributeByte) {
    if (msg == NULL) {
        kPrintStringColour("Log message cannot be NULL\n", attributeByte);
        return;
    }
    int cursorAddress = get_cursor();
    int currentRow = cursorAddress / (2 * MAX_COLS);

    int col = 0;
    kPrintStringColourAt(levelStr, col, currentRow, attributeByte);
    kPrintString(msg);
    kPrintString("\n");
}

void kLogInfo(char* msg) {
    if (level > INFO) {
        return;
    } 

    char colour = 0x02;
    char * levelStr = "INFO: ";
    log(levelStr, msg, colour);
}

void kLogWarning(char* msg) {
    if (level > WARNING) {
        return;
    } 

    char colour = 0xE;
    char * levelStr = "WARNING: ";
    log(levelStr, msg, colour);
}

void kLogError(char* msg) {
    if (level > ERROR) {
        return;
    } 

    char colour = 0x04;
    char * levelStr = "ERROR: ";
    log(levelStr, msg, colour);
}