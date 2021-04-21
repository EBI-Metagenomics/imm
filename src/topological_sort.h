#ifndef TOPOLOGICAL_SORT_H
#define TOPOLOGICAL_SORT_H

#include <stdint.h>

struct imm_state;

int topological_sort(struct imm_state** states, uint16_t nstates, uint16_t ntrans);

#endif
