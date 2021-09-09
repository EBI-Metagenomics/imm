#ifndef IMM_FRAME_COND_H
#define IMM_FRAME_COND_H

#include "imm/export.h"
#include "imm/float.h"
#include "imm/frame_state.h"

struct imm_codon;
struct imm_nuclt_lprob;
struct imm_seq;

struct imm_frame_cond
{
    imm_float loge;
    imm_float log1e;
    struct imm_nuclt_lprob const *nucltp;
    struct imm_codon_marg const *codonm;
};

static inline struct imm_frame_cond
imm_frame_cond_init(struct imm_frame_state const *s)
{
    return (struct imm_frame_cond){s->leps, s->l1eps, s->nucltp, s->codonm};
}

IMM_API imm_float imm_frame_cond_decode(struct imm_frame_cond const *cond,
                                        struct imm_seq const *seq,
                                        struct imm_codon *codon);

IMM_API imm_float imm_frame_cond_loglik(struct imm_frame_cond const *cond,
                                        struct imm_codon const *codon,
                                        struct imm_seq const *seq);

IMM_API imm_float imm_frame_cond_lprob(struct imm_frame_cond const *cond,
                                       struct imm_codon const *codon,
                                       struct imm_seq const *seq);

#endif
