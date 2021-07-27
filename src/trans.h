#ifndef TRANS_H
#define TRANS_H

#include "cco/hnode.h"
#include "cco/node.h"
#include "imm/float.h"
#include "pair.h"

struct trans
{
    struct pair pair;
    imm_float lprob;
    struct cco_node outgoing;
    struct cco_node incoming;
    struct cco_hnode hnode;
};

static inline void trans_init(struct trans *trans, unsigned src, unsigned dst,
                              imm_float lprob)
{
    pair_init(&trans->pair, src, dst);
    trans->lprob = lprob;
    cco_node_init(&trans->outgoing);
    cco_node_init(&trans->incoming);
    cco_hnode_init(&trans->hnode);
}

#endif
