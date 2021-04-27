#ifndef TRANS_H
#define TRANS_H

#include "containers/hnode.h"
#include "containers/node.h"
#include "imm/float.h"
#include "pair.h"

struct trans
{
    struct pair pair;
    imm_float lprob;
    struct node node;
    struct node inode;
    struct hnode hnode;
};

static inline void trans_init(struct trans *trans, uint16_t src, uint16_t dst,
                              imm_float lprob)
{
    pair_init(&trans->pair, src, dst);
    trans->lprob = lprob;
    node_init(&trans->node);
    node_init(&trans->inode);
    hnode_init(&trans->hnode);
}

#endif
