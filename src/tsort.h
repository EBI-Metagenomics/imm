#ifndef TSORT_H
#define TSORT_H

#include <stdint.h>

struct imm_state;

int tsort(struct imm_state **states, uint16_t nstates, uint16_t ntrans);

#endif
