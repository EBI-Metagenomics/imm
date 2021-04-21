#ifndef HMM_H
#define HMM_H

#include "containers/containers.h"
#include "imm/float.h"
#include <stdint.h>

struct imm_abc;
struct imm_dp;
struct imm_hmm;
struct imm_state;

union state_pair
{
    uint16_t ids[2];
    uint32_t key;
};

struct trans
{
    union state_pair pair;
    imm_float        lprob;
    struct node      node;
    struct hnode     hnode;
};

#define STATE_PAIR_INIT(src, dst)                                                                                      \
    {                                                                                                                  \
        .ids[0] = (src), .ids[1] = (dst)                                                                               \
    }

static inline void state_pair_init(union state_pair* pair, uint16_t src, uint16_t dst)
{
    pair->ids[0] = src;
    pair->ids[1] = dst;
}

static inline void trans_init(struct trans* trans, uint16_t src, uint16_t dst, imm_float lprob)
{
    state_pair_init(&trans->pair, src, dst);
    trans->lprob = lprob;
    node_init(&trans->node);
    hnode_init(&trans->hnode);
}

#define MAX_NTRANS (1 << 13)

struct imm_hmm
{
    struct imm_abc const* abc;
    imm_float             start_lprob;
    uint16_t              start_state;

    uint16_t nstates;
    HASH_DECLARE(state_tbl, 11);

    uint16_t ntrans;
    HASH_DECLARE(trans_tbl, 11);
    /* TODO: check if this is wise */
    struct trans trans[MAX_NTRANS];
};

struct imm_abc const* hmm_abc(struct imm_hmm const* hmm);
void                  hmm_add_state(struct imm_hmm* hmm, struct imm_state* mstate);
struct imm_state**    hmm_get_states(struct imm_hmm const* hmm, struct imm_dp const* dp);
struct imm_state*     hmm_state(struct imm_hmm const* hmm, uint16_t state_id);
struct trans* hmm_get_trans(struct imm_hmm const* hmm, struct imm_state const* src, struct imm_state const* dst);

#endif
