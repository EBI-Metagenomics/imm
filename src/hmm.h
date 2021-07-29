#ifndef HMM_H
#define HMM_H

#include "imm/hmm.h"

static inline void hmm_add_state(struct imm_hmm *hmm, struct imm_state *state)
{
    cco_hash_add(hmm->states.tbl, &state->hnode, state->id);
    hmm->states.size++;
}

struct imm_state *hmm_state(struct imm_hmm const *hmm, unsigned state_id);

struct imm_trans *hmm_trans(struct imm_hmm const *hmm,
                            struct imm_state const *src,
                            struct imm_state const *dst);

#endif
