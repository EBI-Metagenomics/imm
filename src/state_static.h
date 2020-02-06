#ifndef IMM_STATE_STATIC_H
#define IMM_STATE_STATIC_H

#include "bug.h"
#include "imm/lprob.h"
#include "imm/report.h"
#include "imm/seq.h"
#include "imm/state.h"

struct imm_state
{
    char const*           name;
    struct imm_abc const* abc;

    imm_state_lprob_t   lprob;
    imm_state_min_seq_t min_seq;
    imm_state_max_seq_t max_seq;
    void*               impl;
};

static inline double state_lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    if (state->abc != imm_seq_get_abc(seq)) {
        imm_error("alphabets must be the same");
        return IMM_LPROB_INVALID;
    }
    return state->lprob(state, seq);
}

#endif
