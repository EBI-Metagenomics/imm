#ifndef HMM_H
#define HMM_H

#include "pair.h"
#include "trans.h"

#define MAX_NTRANS_BITS 13

struct imm_abc;
struct imm_hmm;
struct imm_state;

struct imm_hmm
{
    struct imm_abc const *abc;

    struct
    {
        imm_float lprob;
        unsigned state_id;
    } start;

    struct
    {
        unsigned size;
        HASH_DECLARE(tbl, MAX_NTRANS_BITS - 1);
    } states;

    struct
    {
        unsigned size;
        HASH_DECLARE(tbl, MAX_NTRANS_BITS - 1);
        struct trans data[(1 << MAX_NTRANS_BITS)];
    } transitions;
};

static inline void hmm_add_state(struct imm_hmm *hmm, struct imm_state *state)
{
    hash_add(hmm->states.tbl, &state->hnode, state->id);
    hmm->states.size++;
}

struct imm_state *hmm_state(struct imm_hmm const *hmm, unsigned state_id);

struct trans *hmm_trans(struct imm_hmm const *hmm, struct imm_state const *src,
                        struct imm_state const *dst);

#endif
