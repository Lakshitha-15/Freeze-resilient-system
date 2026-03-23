k#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "log.h"

void log_info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("[INFO] ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

void log_warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("[WARN] ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

void log_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("[ERROR] ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}
