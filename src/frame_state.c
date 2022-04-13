#include "imm/frame_state.h"
#include "error.h"
#include "imm/codon_marg.h"
#include "imm/frame_cond.h"
#include "imm/generics.h"
#include "imm/lprob.h"
#include "imm/nuclt_lprob.h"
#include "logsum.h"
#include "state.h"
#include <assert.h>

static void del(struct imm_state const *state);

static imm_float lprob(struct imm_state const *state,
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

    struct imm_state_vtable vtable = {lprob, IMM_FRAME_STATE, state};
    struct imm_abc const *abc = imm_super(codonm->nuclt);
    state->super = __imm_state_init(id, abc, vtable, span);
}

#define CODON(A, B, C)                                                         \
    (struct imm_codon) { .a = (A), .b = (B), .c = (C) }

#define LP(A, B, C)                                                            \
    imm_codon_marg_lprob(state->codonm, CODON(nucl[A], nucl[B], nucl[C]))

static imm_float joint_seq_len1(struct imm_frame_state const *state,
                                struct imm_seq const *seq)
{
    struct imm_abc const *abc = imm_super(state->codonm->nuclt);
    unsigned nucl[2] = {imm_abc_symbol_idx(abc, imm_seq_str(seq)[0]),
                        imm_abc_any_symbol_idx(abc)};
    unsigned _ = IMM_ARRAY_SIZE(nucl) - 1;

    imm_float c = 2 * state->eps.loge + 2 * state->eps.log1e;

    return c + LOGSUM(LP(0, _, _), LP(_, 0, _), LP(_, _, 0)) - imm_log(3);
}

static imm_float joint_seq_len2(struct imm_frame_state const *state,
                                struct imm_seq const *seq)
{
    struct imm_abc const *abc = imm_super(state->codonm->nuclt);
    unsigned nucl[3] = {imm_abc_symbol_idx(abc, imm_seq_str(seq)[0]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[1]),
                        imm_abc_any_symbol_idx(abc)};
    unsigned _ = IMM_ARRAY_SIZE(nucl) - 1;

    imm_float b_lp0 = __imm_nuclt_lprob_get(state->nucltp, nucl[0]);
    imm_float b_lp1 = __imm_nuclt_lprob_get(state->nucltp, nucl[1]);

    imm_float c0 =
        imm_log(2) + state->eps.loge + state->eps.log1e * 3 - imm_log(3);
    imm_float c1 = 3 * state->eps.loge + state->eps.log1e - imm_log(3);

    imm_float v[3] = {
        c0 + LOGSUM(LP(_, 0, 1), LP(0, _, 1), LP(0, 1, _)),
        c1 + LOGSUM(LP(0, _, _), LP(_, 0, _), LP(_, _, 0)) + b_lp1,
        c1 + LOGSUM(LP(1, _, _), LP(_, 1, _), LP(_, _, 1)) + b_lp0};

    return imm_lprob_sum(3, v);
}

static imm_float joint_seq_len3(struct imm_frame_state const *state,
                                struct imm_seq const *seq)
{
    struct imm_abc const *abc = imm_super(state->codonm->nuclt);
    unsigned nucl[4] = {imm_abc_symbol_idx(abc, imm_seq_str(seq)[0]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[1]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[2]),
                        imm_abc_any_symbol_idx(abc)};
    unsigned _ = IMM_ARRAY_SIZE(nucl) - 1;

    imm_float B[] = {__imm_nuclt_lprob_get(state->nucltp, nucl[0]),
                     __imm_nuclt_lprob_get(state->nucltp, nucl[1]),
                     __imm_nuclt_lprob_get(state->nucltp, nucl[2])};

    imm_float v0 = 4 * state->eps.log1e + LP(0, 1, 2);

    imm_float c1 =
        imm_log(4) + 2 * state->eps.loge + 2 * state->eps.log1e - imm_log(9);
    imm_float c2 = 4 * state->eps.loge - imm_log(9);

    imm_float v1 = LOGSUM(LOGSUM(LP(_, 1, 2), LP(1, _, 2), LP(1, 2, _)) + B[0],
                          LOGSUM(LP(_, 0, 2), LP(0, _, 2), LP(0, 2, _)) + B[1],
                          LOGSUM(LP(_, 0, 1), LP(0, _, 1), LP(0, 1, _)) + B[2]);

    imm_float v2 =
        LOGSUM(LOGSUM(LP(2, _, _), LP(_, 2, _), LP(_, _, 2)) + B[0] + B[1],
               LOGSUM(LP(1, _, _), LP(_, 1, _), LP(_, _, 1)) + B[0] + B[2],
               LOGSUM(LP(0, _, _), LP(_, 0, _), LP(_, _, 0)) + B[1] + B[2]);

    return LOGSUM(v0, c1 + v1, c2 + v2);
}

static imm_float joint_seq_len4(struct imm_frame_state const *state,
                                struct imm_seq const *seq)
{
    struct imm_abc const *abc = imm_super(state->codonm->nuclt);
    unsigned nucl[5] = {imm_abc_symbol_idx(abc, imm_seq_str(seq)[0]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[1]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[2]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[3]),
                        imm_abc_any_symbol_idx(abc)};
    unsigned _ = IMM_ARRAY_SIZE(nucl) - 1;

    imm_float B[] = {__imm_nuclt_lprob_get(state->nucltp, nucl[0]),
                     __imm_nuclt_lprob_get(state->nucltp, nucl[1]),
                     __imm_nuclt_lprob_get(state->nucltp, nucl[2]),
                     __imm_nuclt_lprob_get(state->nucltp, nucl[3])};

    imm_float c0 = state->eps.loge + state->eps.log1e * 3 - imm_log(2);
    imm_float v0 = LOGSUM(LP(1, 2, 3) + B[0], LP(0, 2, 3) + B[1],
                          LP(0, 1, 3) + B[2], LP(0, 1, 2) + B[3]);

    imm_float c1 = 3 * state->eps.loge + state->eps.log1e - imm_log(9);
    imm_float v1 = LOGSUM(LP(_, 2, 3) + B[0] + B[1], LP(_, 1, 3) + B[0] + B[2],
                          LP(_, 1, 2) + B[0] + B[3], LP(_, 0, 3) + B[1] + B[2],
                          LP(_, 0, 2) + B[1] + B[3], LP(_, 0, 1) + B[2] + B[3],
                          LP(2, _, 3) + B[0] + B[1], LP(1, _, 3) + B[0] + B[2],
                          LP(1, _, 2) + B[0] + B[3], LP(0, _, 3) + B[1] + B[2],
                          LP(0, _, 2) + B[1] + B[3], LP(0, _, 1) + B[2] + B[3],
                          LP(2, 3, _) + B[0] + B[1], LP(1, 3, _) + B[0] + B[2],
                          LP(1, 2, _) + B[0] + B[3], LP(0, 3, _) + B[1] + B[2],
                          LP(0, 2, _) + B[1] + B[3], LP(0, 1, _) + B[2] + B[3]);

    return imm_lprob_add(c0 + v0, c1 + v1);
}

static imm_float joint_seq_len5(struct imm_frame_state const *state,
                                struct imm_seq const *seq)
{
    struct imm_abc const *abc = imm_super(state->codonm->nuclt);
    unsigned nucl[6] = {imm_abc_symbol_idx(abc, imm_seq_str(seq)[0]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[1]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[2]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[3]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[4]),
                        imm_abc_any_symbol_idx(abc)};

    imm_float B[] = {__imm_nuclt_lprob_get(state->nucltp, nucl[0]),
                     __imm_nuclt_lprob_get(state->nucltp, nucl[1]),
                     __imm_nuclt_lprob_get(state->nucltp, nucl[2]),
                     __imm_nuclt_lprob_get(state->nucltp, nucl[3]),
                     __imm_nuclt_lprob_get(state->nucltp, nucl[4])};

    imm_float v =
        LOGSUM(LOGSUM(B[0] + B[1] + LP(2, 3, 4), B[0] + B[2] + LP(1, 3, 4)),
               LOGSUM(B[0] + B[3] + LP(1, 2, 4), B[0] + B[4] + LP(1, 2, 3)),
               LOGSUM(B[1] + B[2] + LP(0, 3, 4), B[1] + B[3] + LP(0, 2, 4)),
               LOGSUM(B[1] + B[4] + LP(0, 2, 3), B[2] + B[3] + LP(0, 1, 4)),
               LOGSUM(B[2] + B[4] + LP(0, 1, 3), B[3] + B[4] + LP(0, 1, 2)));

    return 2 * state->eps.loge + 2 * state->eps.log1e - imm_log(10) + v;
}

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
    struct imm_frame_cond cond = imm_frame_cond_init(state);
    return imm_frame_cond_lprob(&cond, codon, seq);
}

static imm_float lprob(struct imm_state const *state, struct imm_seq const *seq)
{
    struct imm_frame_state const *f = state->vtable.derived;

    switch (imm_seq_size(seq))
    {
    case 1:
        return joint_seq_len1(f, seq);
    case 2:
        return joint_seq_len2(f, seq);
    case 3:
        return joint_seq_len3(f, seq);
    case 4:
        return joint_seq_len4(f, seq);
    case 5:
        return joint_seq_len5(f, seq);
    default:
        error(IMM_ILLEGALARG, "sequence not in range");
    }

    return imm_lprob_zero();
}
