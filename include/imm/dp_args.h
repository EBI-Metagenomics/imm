#ifndef IMM_DP_ARGS_H
#define IMM_DP_ARGS_H

#include "imm/float.h"

struct imm_dp_args
{
    unsigned ntrans;
    unsigned nstates;
    struct imm_state **states;
    struct
    {
        struct imm_state const *state;
        imm_float lprob;
    } start;
    struct imm_state const *end_state;
};

static inline struct imm_dp_args
imm_dp_args_init(unsigned ntrans, unsigned nstates, struct imm_state **states,
                 struct imm_state const *start_state, imm_float start_lprob,
                 struct imm_state const *end_state)
{
    return (struct imm_dp_args){.ntrans = ntrans,
                                .nstates = nstates,
                                .states = states,
                                .start.state = start_state,
                                .start.lprob = start_lprob,
                                .end_state = end_state};
}

#endif
