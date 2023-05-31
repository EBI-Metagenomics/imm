#ifndef IMM_FRAME_STATE_H
#define IMM_FRAME_STATE_H

#include "codon_marg.h"
#include "export.h"
#include "frame_epsilon.h"
#include "nuclt_lprob.h"
#include "span.h"
#include "state.h"

struct imm_codon;
struct imm_codon_marg;
struct imm_nuclt_lprob;

struct imm_frame_state
{
  struct imm_state super;
  struct imm_nuclt_lprob const *nucltp;
  struct imm_codon_marg const *codonm;
  float epsilon;
  struct imm_frame_epsilon eps;
};

IMM_API void imm_frame_state_init(struct imm_frame_state *, unsigned id,
                                  struct imm_nuclt_lprob const *,
                                  struct imm_codon_marg const *, float epsilon,
                                  struct imm_span);

IMM_API float imm_frame_state_lposterior(struct imm_frame_state const *,
                                         struct imm_codon const *,
                                         struct imm_seq const *);

IMM_API float imm_frame_state_decode(struct imm_frame_state const *,
                                     struct imm_seq const *,
                                     struct imm_codon *);

IMM_API void imm_frame_state_dump(struct imm_frame_state const *,
                                  FILE *restrict);

#endif
