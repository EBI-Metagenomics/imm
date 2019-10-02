#ifndef IMM_COUNTER_H
#define IMM_COUNTER_H

#include "report.h"
#include <stdlib.h>

struct counter;

struct counter *counter_create(void);
int counter_next(struct counter *counter);
void counter_destroy(struct counter *counter);

#endif
