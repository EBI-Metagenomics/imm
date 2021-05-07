#ifndef IMM_FRAME_STATE_H
#define IMM_FRAME_STATE_H

#include "imm/export.h"
#include "imm/float.h"
#include "imm/state.h"
#include "imm/state_types.h"

struct imm_nuclt_lprob;
struct imm_codon;
struct imm_codon_marg;

struct imm_frame_state
{
    struct imm_state *super;
    struct imm_nuclt_lprob const *nucltp;
    struct imm_codon_marg const *codonm;
    imm_float epsilon;
    imm_float leps;
    imm_float l1eps;
};

IMM_API struct imm_frame_state *
imm_frame_state_new(unsigned id, struct imm_nuclt_lprob const *nucltp,
                    struct imm_codon_marg const *codonm, imm_float epsilon);

static inline void imm_frame_state_del(struct imm_frame_state const *frame)
{
    frame->super->vtable.del(frame->super);
}

IMM_API imm_float imm_frame_state_lposterior(
    struct imm_frame_state const *state, struct imm_codon const *codon,
    struct imm_seq const *seq);

IMM_API imm_float imm_frame_state_decode(struct imm_frame_state const *state,
                                         struct imm_seq const *seq,
                                         struct imm_codon *codon);

static inline struct imm_state *
imm_frame_state_super(struct imm_frame_state *frame)
{
    return frame->super;
}

#endif
