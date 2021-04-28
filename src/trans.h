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
    struct node outgoing;
    struct node incoming;
    struct hnode hnode;
};

static inline void trans_init(struct trans *trans, unsigned src, unsigned dst,
                              imm_float lprob)
{
    pair_init(&trans->pair, src, dst);
    trans->lprob = lprob;
    node_init(&trans->outgoing);
    node_init(&trans->incoming);
    hnode_init(&trans->hnode);
}

#endif
