#ifndef MSTATE_SORT_H
#define MSTATE_SORT_H

#include <inttypes.h>

struct mstate;

void mstate_name_sort(struct mstate const** mstates, uint32_t nstates);
int  mstate_topological_sort(struct mstate const** mstates, uint32_t nstates);

#endif
