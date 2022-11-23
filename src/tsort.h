#ifndef TSORT_H
#define TSORT_H

#include "imm/rc.h"

struct imm_state;

enum imm_rc imm_tsort(unsigned nstates, struct imm_state **states,
                      unsigned start_idx);

#endif
