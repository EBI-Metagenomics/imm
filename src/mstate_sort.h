#ifndef IMM_MSTATE_SORT_H
#define IMM_MSTATE_SORT_H

#include <inttypes.h>

struct mstate;

int mstate_sort(struct mstate const** mstates, uint32_t nstates);

#endif
