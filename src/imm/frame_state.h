#ifndef IMM_FRAME_STATE_H
#define IMM_FRAME_STATE_H

#include "imm/codon_marg.h"
#include "imm/export.h"
#include "imm/float.h"
#include "imm/frame_epsilon.h"
#include "imm/nuclt_lprob.h"
#include "imm/state.h"

struct imm_codon;
struct imm_codon_marg;
struct imm_nuclt_lprob;

struct imm_frame_state
{
    struct imm_state super;
    struct imm_nuclt_lprob const *nucltp;
    struct imm_codon_marg const *codonm;
    imm_float epsilon;
    struct imm_frame_epsilon eps;
};

IMM_API void imm_frame_state_init(struct imm_frame_state *state, unsigned id,
                                  struct imm_nuclt_lprob const *nucltp,
                                  struct imm_codon_marg const *codonm,
                                  imm_float epsilon);

IMM_API imm_float imm_frame_state_lposterior(
    struct imm_frame_state const *state, struct imm_codon const *codon,
    struct imm_seq const *seq);

IMM_API imm_float imm_frame_state_decode(struct imm_frame_state const *state,
                                         struct imm_seq const *seq,
                                         struct imm_codon *codon);

static inline struct imm_state *
imm_frame_state_super(struct imm_frame_state *frame)
{
    return &frame->super;
}

static inline struct imm_state const *
imm_frame_state_super_c(struct imm_frame_state const *frame)
{
    return &frame->super;
}

#endif
