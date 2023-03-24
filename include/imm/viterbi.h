#ifndef IMM_VITERBI_H
#define IMM_VITERBI_H

#include "imm/btrans.h"
#include "imm/cell.h"
#include "imm/float.h"
#include "imm/range.h"

struct imm_dp;
struct imm_task;

struct imm_viterbi
{
    struct imm_dp const *dp;
    struct imm_task *task;
    unsigned seqlen;
    unsigned unsafe_state;
};

unsigned imm_viterbi_nstates(struct imm_viterbi const *);
unsigned imm_viterbi_ntrans(struct imm_viterbi const *, unsigned dst);
unsigned imm_viterbi_source(struct imm_viterbi const *, unsigned dst,
                            unsigned trans);
struct imm_range imm_viterbi_range(struct imm_viterbi const *, unsigned state);
imm_float imm_viterbi_get_score(struct imm_viterbi const *, unsigned row,
                                unsigned state, unsigned len);
void imm_viterbi_set_score(struct imm_viterbi const *, struct imm_cell,
                           imm_float);
imm_float imm_viterbi_trans_score(struct imm_viterbi const *, unsigned dst,
                                  unsigned trans);
unsigned imm_viterbi_start_state(struct imm_viterbi const *);
imm_float imm_viterbi_start_lprob(struct imm_viterbi const *);
imm_float imm_viterbi_emission(struct imm_viterbi const *x, unsigned row,
                               unsigned state, unsigned len, unsigned min_len);

#endif
