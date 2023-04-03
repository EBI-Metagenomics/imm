#ifndef IMM_FRAME_COND_H
#define IMM_FRAME_COND_H

#include "export.h"
#include "frame_epsilon.h"
#include "frame_state.h"

struct imm_codon;
struct imm_nuclt_lprob;
struct imm_seq;

struct imm_frame_cond
{
  struct imm_frame_epsilon eps;
  struct imm_nuclt_lprob const *nucltp;
  struct imm_codon_marg const *codonm;
};

IMM_API struct imm_frame_cond imm_frame_cond(struct imm_frame_state const *);
IMM_API float imm_frame_cond_decode(struct imm_frame_cond const *,
                                    struct imm_seq const *, struct imm_codon *);
IMM_API float imm_frame_cond_loglik(struct imm_frame_cond const *,
                                    struct imm_codon const *,
                                    struct imm_seq const *);
IMM_API float imm_frame_cond_lprob(struct imm_frame_cond const *,
                                   struct imm_codon const *,
                                   struct imm_seq const *);

#endif
