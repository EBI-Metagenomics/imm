#include "imm/frame_state.h"
#include "error.h"
#include "frame_state_15.h"
#include "frame_state_24.h"
#include "frame_state_33.h"
#include "imm/codon_marg.h"
#include "imm/frame_cond.h"
#include "imm/generics.h"
#include "imm/lprob.h"
#include "imm/nuclt_lprob.h"
#include "logsum.h"
#include "state.h"
#include <assert.h>

static imm_float lprob15(struct imm_state const *state,
                         struct imm_seq const *seq);
static imm_float lprob24(struct imm_state const *state,
                         struct imm_seq const *seq);
static imm_float lprob33(struct imm_state const *state,
                         struct imm_seq const *seq);

void imm_frame_state_init(struct imm_frame_state *state, unsigned id,
                          struct imm_nuclt_lprob const *nucltp,
                          struct imm_codon_marg const *codonm,
                          imm_float epsilon, struct imm_span span)
{
    assert(nucltp->nuclt == codonm->nuclt);
    state->nucltp = nucltp;
    state->codonm = codonm;
    state->epsilon = epsilon;
    state->eps = imm_frame_epsilon(epsilon);

    assert((span.min == 1 && span.max == 5) ||
           (span.min == 2 && span.max == 4) ||
           (span.min == 3 && span.max == 3));

    struct imm_state_vtable vtable = {NULL, IMM_FRAME_STATE, state};
    if (span.min == 1 && span.max == 5) vtable.lprob = lprob15;
    if (span.min == 2 && span.max == 4) vtable.lprob = lprob24;
    if (span.min == 3 && span.max == 3) vtable.lprob = lprob33;

    struct imm_abc const *abc = imm_super(codonm->nuclt);
    __imm_state_init(&state->super, id, abc, vtable, span);
}

#define CODON(A, B, C)                                                         \
    (struct imm_codon) { .a = (A), .b = (B), .c = (C) }

#define lf(A, B, C)                                                            \
    imm_codon_marg_lprob(state->codonm, CODON(nucl[A], nucl[B], nucl[C]))

imm_float imm_frame_state_decode(struct imm_frame_state const *state,
                                 struct imm_seq const *seq,
                                 struct imm_codon *codon)
{
    struct imm_frame_cond cond = imm_frame_cond_init(state);
    return imm_frame_cond_decode(&cond, seq, codon);
}

imm_float imm_frame_state_lposterior(struct imm_frame_state const *state,
                                     struct imm_codon const *codon,
                                     struct imm_seq const *seq)
{
    struct imm_span span = imm_frame_state_super_c(state)->span;
    (void)span;
    assert((span.min == 1 && span.max == 5) ||
           (span.min == 2 && span.max == 4) ||
           (span.min == 3 && span.max == 3));
    struct imm_frame_cond cond = imm_frame_cond_init(state);
    return imm_frame_cond_lprob(&cond, codon, seq);
}

static imm_float lprob15(struct imm_state const *state,
                         struct imm_seq const *seq)
{
    struct imm_frame_state const *f = state->vtable.derived;

    switch (imm_seq_size(seq))
    {
    case 1:
        return imm_joint_n1_15(f, seq);
    case 2:
        return imm_joint_n2_15(f, seq);
    case 3:
        return imm_joint_n3_15(f, seq);
    case 4:
        return imm_joint_n4_15(f, seq);
    case 5:
        return imm_joint_n5_15(f, seq);
        // default:
        //     error(IMM_SEQ_OUT_OF_RANGE);
    }

    return imm_lprob_zero();
}

static imm_float lprob24(struct imm_state const *state,
                         struct imm_seq const *seq)
{
    struct imm_frame_state const *f = state->vtable.derived;

    switch (imm_seq_size(seq))
    {
    case 2:
        return imm_joint_n2_24(f, seq);
    case 3:
        return imm_joint_n3_24(f, seq);
    case 4:
        return imm_joint_n4_24(f, seq);
        // default:
        //     error(IMM_SEQ_OUT_OF_RANGE);
    }

    return imm_lprob_zero();
}

static imm_float lprob33(struct imm_state const *state,
                         struct imm_seq const *seq)
{
    struct imm_frame_state const *f = state->vtable.derived;

    switch (imm_seq_size(seq))
    {
    case 3:
        return imm_joint_n3_33(f, seq);
        // default:
        //     error(IMM_SEQ_OUT_OF_RANGE);
    }

    return imm_lprob_zero();
}
