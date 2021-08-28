#ifndef DP_DP_H
#define DP_DP_H

#include "imm/float.h"

struct imm_dp;
struct imm_state;

struct dp_args
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

static inline struct dp_args dp_args(unsigned ntrans, unsigned nstates,
                                     struct imm_state **states,
                                     struct imm_state const *start_state,
                                     imm_float start_lprob,
                                     struct imm_state const *end_state)
{
    return (struct dp_args){.ntrans = ntrans,
                            .nstates = nstates,
                            .states = states,
                            .start.state = start_state,
                            .start.lprob = start_lprob,
                            .end_state = end_state};
}

enum imm_rc dp_reset(struct imm_dp *dp, struct dp_args const *args);

#endif
