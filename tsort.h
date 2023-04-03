#ifndef IMM_TSORT_H
#define IMM_TSORT_H

#include "rc.h"

struct imm_state;

int imm_tsort(unsigned nstates, struct imm_state **states, unsigned start_idx);

#endif
