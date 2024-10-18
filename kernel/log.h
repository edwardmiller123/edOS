#ifndef LOG
#define LOG

typedef enum LogLevel {
    FATAL,
    ERROR,
    WARNING,
    INFO,
    DEBUG,
} LogLevel;

// setLogLevel sets the global logging level
void setLogLevel(LogLevel logLevel);

void kLogDebug(char *msg);

void kLogInfo(char* msg);

void kLogWarning(char* msg);

void kLogError(char* msg);

void kLogFatal(char *msg);

// kLogf logs a formatted message at the given log level
void kLogf(LogLevel level, char *msg, int args[], int argCount);

#endif