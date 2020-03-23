#ifndef IMM_DP_TRANS_TABLE_H
#define IMM_DP_TRANS_TABLE_H

struct mstate;
struct state_idx;

struct dp_trans_table const* dp_trans_table_create(struct mstate const* const* mstates,
                                                   unsigned                    nstates,
                                                   struct state_idx*           state_idx);
void                         dp_trans_table_destroy(struct dp_trans_table const* transition);

#endif
