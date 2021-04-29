#ifndef TSORT_H
#define TSORT_H

struct imm_state;

int tsort(struct imm_state **states, unsigned nstates, unsigned ntrans);

#endif
