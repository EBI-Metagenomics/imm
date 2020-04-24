#ifndef HMM_H
#define HMM_H

#include <stdio.h>

struct imm_abc;
struct imm_dp;
struct imm_hmm;
struct imm_io;
struct mstate;

struct imm_abc const* hmm_abc(struct imm_hmm const* hmm);
void                  hmm_add_mstate(struct imm_hmm* hmm, struct mstate* mstate);

#endif
