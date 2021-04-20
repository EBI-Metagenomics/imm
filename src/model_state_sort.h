#ifndef MODEL_STATE_SORT_H
#define MODEL_STATE_SORT_H

#include <inttypes.h>

struct model_state;
struct imm_state;

void model_state_name_sort(struct model_state const** mstates, uint16_t nstates);
int  model_state_topological_sort(struct imm_state const** states, uint16_t nstates);

#endif
