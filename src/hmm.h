#ifndef HMM_H
#define HMM_H

#include "pair.h"
#include "start.h"
#include "trans.h"

struct imm_abc;
struct imm_hmm;
struct imm_state;

struct imm_hmm
{
    struct imm_abc const *abc;
    struct start start;

    struct
    {
        uint16_t size;
        HASH_DECLARE(tbl, 11);
    } states;

    struct
    {
        uint16_t size;
        HASH_DECLARE(tbl, 11);
        size_t capacity;
        struct trans *data;
    } transitions;
};

static inline void hmm_add_state(struct imm_hmm *hmm, struct imm_state *state)
{
    hash_add(hmm->states.tbl, &state->hnode, state->id);
    hmm->states.size++;
}

static inline struct imm_state *hmm_state(struct imm_hmm const *hmm,
                                          uint16_t state_id)
{
    struct imm_state *state = NULL;
    hash_for_each_possible(hmm->states.tbl, state, hnode, state_id)
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
    struct pair pair = PAIR_INIT(src->id, dst->id);
    hash_for_each_possible(hmm->transitions.tbl, trans, hnode, pair.id.key)
    {
        if (trans->pair.id.src == src->id && trans->pair.id.dst == dst->id)
            return trans;
    }
    return NULL;
}

#endif
