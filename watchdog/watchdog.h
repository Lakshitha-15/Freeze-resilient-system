#ifndef WATCHDOG_H
#define WATCHDOG_H
#include "../runtime/container.h"

int watchdog_start();
int watchdog_add(Container *c);
#endif
