#ifndef _NHMM_STATE_NORMAL_H
#define _NHMM_STATE_NORMAL_H

#include "state/state.h"

struct normal_state
{
    double *emission;
};

void normal_state_create(struct nhmm_state *state, double *emission);
double normal_state_emission(struct nhmm_state *state, const char* x, size_t xlen);
void normal_state_destroy(struct nhmm_state *state);

#endif
