#ifndef TSORT_H
#define TSORT_H

#include "imm/rc.h"

struct imm_state;

int imm_tsort(unsigned nstates, struct imm_state **states, unsigned start_idx);

#endif
