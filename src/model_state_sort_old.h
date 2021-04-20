#ifndef MODEL_STATE_SORT_OLD_H
#define MODEL_STATE_SORT_OLD_H

#include <inttypes.h>

struct model_state;

void model_state_name_sort_old(struct model_state const** mstates, uint16_t nstates);
int  model_state_topological_sort_old(struct model_state const** mstates, uint16_t nstates);

#endif
