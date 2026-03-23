#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "utils.h"

int str_to_long(const char *s, long *result) {
    char *endptr;
    *result = strtol(s, &endptr, 10);
    return *endptr == '\0' || *endptr == '\n';
}

long get_us_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000LL + tv.tv_usec;
}
