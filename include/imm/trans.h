#ifndef IMM_TRANS_H
#define IMM_TRANS_H

#include "cco.h"
#include "pair.h"
#include <stdint.h>

typedef uint16_t imm_trans_idx_t;

#define IMM_TRANS_NULL_IDX UINT16_MAX

#define IMM_TRANS_MAX_SIZE 15

struct imm_trans
{
    struct imm_pair pair;
    imm_float lprob;
    struct cco_node outgoing;
    struct cco_node incoming;
    struct cco_hnode hnode;
};

static inline void trans_init(struct imm_trans *trans, unsigned src,
                              unsigned dst, imm_float lprob)
{
    imm_pair_init(&trans->pair, src, dst);
    trans->lprob = lprob;
    cco_node_init(&trans->outgoing);
    cco_node_init(&trans->incoming);
    cco_hnode_init(&trans->hnode);
}

#endif
