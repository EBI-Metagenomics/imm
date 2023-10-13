#include "trans.h"

void imm_trans_init(struct imm_trans *trans, int src, int dst, float lprob)
{
  trans->pair = imm_pair(src, dst);
  trans->lprob = lprob;
  imm_list_init(&trans->outgoing);
  imm_list_init(&trans->incoming);
  cco_hnode_init(&trans->hnode);
}
