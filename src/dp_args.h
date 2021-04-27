#ifndef DP_ARGS_H
#define DP_ARGS_H

#include "imm/float.h"
#include "start.h"
#include <stdint.h>

struct imm_state;

struct dp_args
{
    uint16_t ntransitions;
    uint16_t nstates;
    struct imm_state **states;
    struct
    {
        struct imm_state const *state;
        imm_float lprob;
    } start;
    struct imm_state const *end_state;
};

static void dp_args_init(struct dp_args *args, uint16_t ntransitions,
                         uint16_t nstates, struct imm_state **states,
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
