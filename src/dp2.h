#ifndef IMM_DP2_H
#define IMM_DP2_H

struct mstate;
struct seq_code;
struct imm_abc;

struct dp2 const* dp2_create(struct imm_abc const* abc, struct mstate const* const* mstates,
                             unsigned const nstates);
void              dp2_destroy(struct dp2 const* dp);

#endif
