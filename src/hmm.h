#ifndef HMM_H
#define HMM_H

#include "start.h"
#include "state_pair.h"
#include "trans.h"

struct imm_abc;
/* struct imm_dp; */
struct imm_hmm;
struct imm_state;

#define HMM_MAX_NTRANS (1 << 13)

struct imm_hmm
{
    struct imm_abc const *abc;
    struct start start;

    uint16_t nstates;
    HASH_DECLARE(state_tbl, 11);

    uint16_t ntrans;
    HASH_DECLARE(trans_tbl, 11);
    size_t trans_capacity;
    struct trans *trans;
};

static inline void hmm_add_state(struct imm_hmm *hmm, struct imm_state *state)
{
    hash_add(hmm->state_tbl, &state->hnode, state->id);
    hmm->nstates++;
}

static inline struct imm_state *hmm_state(struct imm_hmm const *hmm,
                                          uint16_t state_id)
{
    struct imm_state *state = NULL;
    hash_for_each_possible(hmm->state_tbl, state, hnode, state_id)
    {
        if (state->id == state_id)
            return state;
    }
    return NULL;
}

static inline struct trans *hmm_trans(struct imm_hmm const *hmm,
                                      struct imm_state const *src,
                                      struct imm_state const *dst)
{
    struct trans *trans = NULL;
    union state_pair pair = STATE_PAIR_INIT(src->id, dst->id);
    hash_for_each_possible(hmm->trans_tbl, trans, hnode, pair.key)
    {
        if (trans->pair.ids[0] == src->id && trans->pair.ids[1] == dst->id)
            return trans;
    }
    return NULL;
}

#endif
