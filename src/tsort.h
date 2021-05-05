#ifndef TSORT_H
#define TSORT_H

struct imm_state;

int tsort(unsigned nstates, struct imm_state **states, unsigned start_idx);

#endif
