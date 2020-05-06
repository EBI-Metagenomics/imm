#ifndef HMM_H
#define HMM_H

#include <stdio.h>

struct imm_abc;
struct imm_dp;
struct imm_hmm;
struct mstate;

struct imm_abc const*       hmm_abc(struct imm_hmm const* hmm);
void                        hmm_add_mstate(struct imm_hmm* hmm, struct mstate* mstate);
struct mstate const* const* hmm_get_mstates(struct imm_hmm const* hmm, struct imm_dp const* dp);

#endif
