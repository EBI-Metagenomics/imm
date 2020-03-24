#ifndef IMM_DP2_H
#define IMM_DP2_H

struct mstate;
struct seq_code;

struct dp2 const* dp2_create(struct mstate const* const* mstates, unsigned const nstates,
                             struct seq_code const* seq_code);
void              dp2_destroy(struct dp2 const* dp);

#endif
