#ifndef _NHMM_STATE_NORMAL_H
#define _NHMM_STATE_NORMAL_H

#include "state/state.h"

struct normal_state
{
    double *emission;
};

struct normal_state *create_normal_state(double *emission);
void destroy_normal_state(struct nhmm_state *s);

#endif
