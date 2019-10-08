#ifndef IMM_COUNTER_H
#define IMM_COUNTER_H

#include "src/imm/hide.h"

struct counter;

HIDE struct counter *counter_create(void);
HIDE int counter_next(struct counter *counter);
HIDE void counter_destroy(struct counter *counter);

#endif
