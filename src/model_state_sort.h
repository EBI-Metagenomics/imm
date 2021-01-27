#ifndef MODEL_STATE_SORT_H
#define MODEL_STATE_SORT_H

#include <inttypes.h>

struct model_state;

void model_state_name_sort(struct model_state const** mstates, uint32_t nstates);
int  model_state_topological_sort(struct model_state const** mstates, uint32_t nstates);

#endif