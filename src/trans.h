#ifndef TRANS_H
#define TRANS_H

#include "containers/hnode.h"
#include "containers/node.h"
#include "imm/float.h"
#include "state_pair.h"

struct trans
{
    union state_pair pair;
    imm_float lprob;
    struct node node;
    struct hnode hnode;
};

static inline void trans_init(struct trans *trans, uint16_t src, uint16_t dst,
                              imm_float lprob)
{
    state_pair_init(&trans->pair, src, dst);
    trans->lprob = lprob;
    node_init(&trans->node);
    hnode_init(&trans->hnode);
}

#endif
