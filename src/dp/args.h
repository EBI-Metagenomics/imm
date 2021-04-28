#ifndef DP_ARGS_H
#define DP_ARGS_H

#include "imm/float.h"
#include "start.h"

struct imm_state;

struct dp_args
{
    unsigned ntransitions;
    unsigned nstates;
    struct imm_state **states;
    struct
    {
        struct imm_state const *state;
        imm_float lprob;
    } start;
    struct imm_state const *end_state;
};

static inline void dp_args_init(struct dp_args *args, unsigned ntransitions,
                                unsigned nstates, struct imm_state **states,
                                struct imm_state const *start_state,
                                imm_float start_lprob,
                                struct imm_state const *end_state)
{
    args->ntransitions = ntransitions;
    args->nstates = nstates;
    args->states = states;
    args->start.state = start_state;
    args->start.lprob = start_lprob;
    args->end_state = end_state;
}

#endif
