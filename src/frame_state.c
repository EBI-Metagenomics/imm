#include "imm/frame_state.h"
#include "error.h"
#include "imm/codon_marg.h"
#include "imm/generics.h"
#include "imm/lprob.h"
#include "imm/nuclt_lprob.h"
#include "state.h"
#include <assert.h>

static void del(struct imm_state const *state);

static imm_float lprob(struct imm_state const *state,
                       struct imm_seq const *seq);

void imm_frame_state_init(struct imm_frame_state *state, unsigned id,
                          struct imm_nuclt_lprob const *nucltp,
                          struct imm_codon_marg const *codonm,
                          imm_float epsilon)
{
    assert(nucltp->nuclt == codonm->nuclt);
    state->nucltp = nucltp;
    state->codonm = codonm;
    state->epsilon = epsilon;
    state->leps = imm_log(epsilon);
    state->l1eps = imm_log(1 - epsilon);

    struct imm_state_vtable vtable = {lprob, IMM_FRAME_STATE, state};
    struct imm_abc const *abc = imm_super(codonm->nuclt);
    state->super = __imm_state_init(id, abc, vtable, IMM_SPAN(1, 5));
}

static inline imm_float logaddexp3(imm_float const a, imm_float const b,
                                   imm_float const c)
{
    return imm_lprob_add(imm_lprob_add(a, b), c);
}

#define CODON(A, B, C)                                                         \
    (struct imm_codon) { .a = (A), .b = (B), .c = (C) }

#define LP(A, B, C)                                                            \
    imm_codon_marg_lprob(state->codonm, CODON(nucl[A], nucl[B], nucl[C]))

#define NUMARGS(...) (sizeof((int[]){__VA_ARGS__}) / sizeof(int))

#define ARR(...)                                                               \
    NUMARGS(__VA_ARGS__), (imm_float[NUMARGS(__VA_ARGS__)]) { __VA_ARGS__ }

#define LOGSUM(...)                                                            \
    imm_lprob_sum(NUMARGS(__VA_ARGS__),                                        \
                  (imm_float[NUMARGS(__VA_ARGS__)]){__VA_ARGS__})

static imm_float joint_seq_len1(struct imm_frame_state const *state,
                                struct imm_seq const *seq)
{
    struct imm_abc const *abc = imm_super(state->codonm->nuclt);
    unsigned nucl[2] = {imm_abc_symbol_idx(abc, imm_seq_str(seq)[0]),
                        imm_abc_any_symbol_idx(abc)};
    unsigned _ = IMM_ARRAY_SIZE(nucl) - 1;

    imm_float c = 2 * state->leps + 2 * state->l1eps;

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

    imm_float c0 = imm_log(2) + state->leps + state->l1eps * 3 - imm_log(3);
    imm_float c1 = 3 * state->leps + state->l1eps - imm_log(3);

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

    imm_float v0 = 4 * state->l1eps + LP(0, 1, 2);

    imm_float c1 = imm_log(4) + 2 * state->leps + 2 * state->l1eps - imm_log(9);
    imm_float c2 = 4 * state->leps - imm_log(9);

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

    imm_float c0 = state->leps + state->l1eps * 3 - imm_log(2);
    imm_float v0 = LOGSUM(LP(1, 2, 3) + B[0], LP(0, 2, 3) + B[1],
                          LP(0, 1, 3) + B[2], LP(0, 1, 2) + B[3]);

    imm_float c1 = 3 * state->leps + state->l1eps - imm_log(9);
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

    return 2 * state->leps + 2 * state->l1eps - imm_log(10) + v;
}

static imm_float lprob_frag_given_codon1(struct imm_frame_state const *state,
                                         struct imm_seq const *seq,
                                         struct imm_codon const *codon)
{
    imm_float loge = state->leps;
    imm_float log1e = state->l1eps;

    unsigned const *x = codon->idx;

    unsigned z[1] = {imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[0])};

    imm_float c = 2 * loge + 2 * log1e;

    return c + imm_log((x[0] == z[0]) + (x[1] == z[0]) + (x[2] == z[0])) -
           imm_log(3);
}

static imm_float lprob_frag_given_codon2(struct imm_frame_state const *state,
                                         struct imm_seq const *seq,
                                         struct imm_codon const *codon)
{
    imm_float loge = state->leps;
    imm_float log1e = state->l1eps;

    unsigned const *x = codon->idx;

    unsigned z[2] = {imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[0]),
                     imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[1])};

    imm_float lprob_z1 = __imm_nuclt_lprob_get(state->nucltp, z[0]);
    imm_float lprob_z2 = __imm_nuclt_lprob_get(state->nucltp, z[1]);

    imm_float c1 = imm_log(2) + loge + log1e * 3 - imm_log(3);
    imm_float v0 = c1 + imm_log((x[1] == z[0]) * (x[2] == z[1]) +
                                (x[0] == z[0]) * (x[2] == z[1]) +
                                (x[0] == z[0]) * (x[1] == z[1]));

    imm_float c2 = 3 * loge + log1e - imm_log(3);

    imm_float v1 = c2 +
                   imm_log((x[0] == z[0]) + (x[1] == z[0]) + (x[2] == z[0])) +
                   lprob_z2;
    imm_float v2 = c2 +
                   imm_log((x[0] == z[1]) + (x[1] == z[1]) + (x[2] == z[1])) +
                   lprob_z1;

    return logaddexp3(v0, v1, v2);
}

static imm_float lprob_frag_given_codon3(struct imm_frame_state const *state,
                                         struct imm_seq const *seq,
                                         struct imm_codon const *codon)
{
    imm_float loge = state->leps;
    imm_float log1e = state->l1eps;

    unsigned const *x = codon->idx;

    unsigned z[3] = {imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[0]),
                     imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[1]),
                     imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[2])};

    imm_float lprob_z1 = __imm_nuclt_lprob_get(state->nucltp, z[0]);
    imm_float lprob_z2 = __imm_nuclt_lprob_get(state->nucltp, z[1]);
    imm_float lprob_z3 = __imm_nuclt_lprob_get(state->nucltp, z[2]);

    imm_float v0 =
        4 * log1e + imm_log((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[2]));

    imm_float c1 = imm_log(4) + 2 * loge + 2 * log1e - imm_log(9);

    imm_float v1 = c1 +
                   imm_log((x[1] == z[1]) * (x[2] == z[2]) +
                           (x[0] == z[1]) * (x[2] == z[2]) +
                           (x[0] == z[1]) * (x[1] == z[2])) +
                   lprob_z1;

    imm_float v2 = c1 +
                   imm_log((x[1] == z[0]) * (x[2] == z[2]) +
                           (x[0] == z[0]) * (x[2] == z[2]) +
                           (x[0] == z[0]) * (x[1] == z[2])) +
                   lprob_z2;

    imm_float v3 = c1 +
                   imm_log((x[1] == z[0]) * (x[2] == z[1]) +
                           (x[0] == z[0]) * (x[2] == z[1]) +
                           (x[0] == z[0]) * (x[1] == z[1])) +
                   lprob_z3;

    imm_float c2 = 4 * loge - imm_log(9);

    imm_float v4 = c2 +
                   imm_log((x[0] == z[2]) + (x[1] == z[2]) + (x[2] == z[2])) +
                   lprob_z1 + lprob_z2;
    imm_float v5 = c2 +
                   imm_log((x[0] == z[1]) + (x[1] == z[1]) + (x[2] == z[1])) +
                   lprob_z1 + lprob_z3;
    imm_float v6 = c2 +
                   imm_log((x[0] == z[0]) + (x[1] == z[0]) + (x[2] == z[0])) +
                   lprob_z2 + lprob_z3;

    return LOGSUM(v0, v1, v2, v3, v4, v5, v6);
}

static imm_float lprob_frag_given_codon4(struct imm_frame_state const *state,
                                         struct imm_seq const *seq,
                                         struct imm_codon const *codon)
{
    imm_float loge = state->leps;
    imm_float log1e = state->l1eps;

    unsigned const *x = codon->idx;

    unsigned z[4] = {imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[0]),
                     imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[1]),
                     imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[2]),
                     imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[3])};

    imm_float lprob_z1 = __imm_nuclt_lprob_get(state->nucltp, z[0]);
    imm_float lprob_z2 = __imm_nuclt_lprob_get(state->nucltp, z[1]);
    imm_float lprob_z3 = __imm_nuclt_lprob_get(state->nucltp, z[2]);
    imm_float lprob_z4 = __imm_nuclt_lprob_get(state->nucltp, z[3]);

    imm_float v0 = LOGSUM(
        imm_log((x[0] == z[1]) * (x[1] == z[2]) * (x[2] == z[3])) + lprob_z1,
        imm_log((x[0] == z[0]) * (x[1] == z[2]) * (x[2] == z[3])) + lprob_z2,
        imm_log((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[3])) + lprob_z3,
        imm_log((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[2])) + lprob_z4);

    imm_float v1 =
        LOGSUM(imm_log((x[1] == z[2]) * (x[2] == z[3])) + lprob_z1 + lprob_z2,
               imm_log((x[1] == z[1]) * (x[2] == z[3])) + lprob_z1 + lprob_z3,
               imm_log((x[1] == z[1]) * (x[2] == z[2])) + lprob_z1 + lprob_z4,
               imm_log((x[1] == z[0]) * (x[2] == z[3])) + lprob_z2 + lprob_z3,
               imm_log((x[1] == z[0]) * (x[2] == z[2])) + lprob_z2 + lprob_z4,
               imm_log((x[1] == z[0]) * (x[2] == z[1])) + lprob_z3 + lprob_z4,
               imm_log((x[0] == z[2]) * (x[2] == z[3])) + lprob_z1 + lprob_z2,
               imm_log((x[0] == z[1]) * (x[2] == z[3])) + lprob_z1 + lprob_z3,
               imm_log((x[0] == z[1]) * (x[2] == z[2])) + lprob_z1 + lprob_z4,
               imm_log((x[0] == z[0]) * (x[2] == z[3])) + lprob_z2 + lprob_z3,
               imm_log((x[0] == z[0]) * (x[2] == z[2])) + lprob_z2 + lprob_z4,
               imm_log((x[0] == z[0]) * (x[2] == z[1])) + lprob_z3 + lprob_z4,
               imm_log((x[0] == z[2]) * (x[1] == z[3])) + lprob_z1 + lprob_z2,
               imm_log((x[0] == z[1]) * (x[1] == z[3])) + lprob_z1 + lprob_z3,
               imm_log((x[0] == z[1]) * (x[1] == z[2])) + lprob_z1 + lprob_z4,
               imm_log((x[0] == z[0]) * (x[1] == z[3])) + lprob_z2 + lprob_z3,
               imm_log((x[0] == z[0]) * (x[1] == z[2])) + lprob_z2 + lprob_z4,
               imm_log((x[0] == z[0]) * (x[1] == z[1])) + lprob_z3 + lprob_z4);

    return imm_lprob_add(loge + log1e * 3 - imm_log(2) + v0,
                         3 * loge + log1e - imm_log(9) + v1);
}

static imm_float lprob_frag_given_codon5(struct imm_frame_state const *state,
                                         struct imm_seq const *seq,
                                         struct imm_codon const *codon)
{
    imm_float loge = state->leps;
    imm_float log1e = state->l1eps;

    unsigned const *x = codon->idx;

    unsigned z[5] = {imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[0]),
                     imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[1]),
                     imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[2]),
                     imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[3]),
                     imm_abc_symbol_idx(seq->abc, imm_seq_str(seq)[4])};

    imm_float lprob_z1 = __imm_nuclt_lprob_get(state->nucltp, z[0]);
    imm_float lprob_z2 = __imm_nuclt_lprob_get(state->nucltp, z[1]);
    imm_float lprob_z3 = __imm_nuclt_lprob_get(state->nucltp, z[2]);
    imm_float lprob_z4 = __imm_nuclt_lprob_get(state->nucltp, z[3]);
    imm_float lprob_z5 = __imm_nuclt_lprob_get(state->nucltp, z[4]);

    imm_float v =
        LOGSUM(lprob_z1 + lprob_z2 +
                   imm_log((x[0] == z[2]) * (x[1] == z[3]) * (x[2] == z[4])),
               lprob_z1 + lprob_z3 +
                   imm_log((x[0] == z[1]) * (x[1] == z[3]) * (x[2] == z[4])),
               lprob_z1 + lprob_z4 +
                   imm_log((x[0] == z[1]) * (x[1] == z[2]) * (x[2] == z[4])),
               lprob_z1 + lprob_z5 +
                   imm_log((x[0] == z[1]) * (x[1] == z[2]) * (x[2] == z[3])),
               lprob_z2 + lprob_z3 +
                   imm_log((x[0] == z[0]) * (x[1] == z[3]) * (x[2] == z[4])),
               lprob_z2 + lprob_z4 +
                   imm_log((x[0] == z[0]) * (x[1] == z[2]) * (x[2] == z[4])),
               lprob_z2 + lprob_z5 +
                   imm_log((x[0] == z[0]) * (x[1] == z[2]) * (x[2] == z[3])),
               lprob_z3 + lprob_z4 +
                   imm_log((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[4])),
               lprob_z3 + lprob_z5 +
                   imm_log((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[3])),
               lprob_z4 + lprob_z5 +
                   imm_log((x[0] == z[0]) * (x[1] == z[1]) * (x[2] == z[2])), );

    return 2 * loge + 2 * log1e - imm_log(10) + v;
}

imm_float imm_frame_state_decode(struct imm_frame_state const *state,
                                 struct imm_seq const *seq,
                                 struct imm_codon *codon)
{
    unsigned const n = imm_nuclt_size(state->nucltp->nuclt);

    imm_float max_lprob = imm_lprob_zero();
    struct imm_codon tmp;
    tmp.nuclt = state->nucltp->nuclt;

    for (unsigned i0 = 0; i0 < n; ++i0)
    {
        for (unsigned i1 = 0; i1 < n; ++i1)
        {
            for (unsigned i2 = 0; i2 < n; ++i2)
            {

                tmp.a = i0;
                tmp.b = i1;
                tmp.c = i2;
                imm_float lprob = imm_frame_state_lposterior(state, &tmp, seq);

                if (lprob >= max_lprob)
                {
                    max_lprob = lprob;
                    codon->a = tmp.a;
                    codon->b = tmp.b;
                    codon->c = tmp.c;
                }
            }
        }
    }
    return max_lprob;
}

imm_float imm_frame_state_lposterior(struct imm_frame_state const *state,
                                     struct imm_codon const *codon,
                                     struct imm_seq const *seq)
{
    imm_float lprob = imm_lprob_zero();

    switch (imm_seq_size(seq))
    {
    case 1:
        lprob = lprob_frag_given_codon1(state, seq, codon);
        break;
    case 2:
        lprob = lprob_frag_given_codon2(state, seq, codon);
        break;
    case 3:
        lprob = lprob_frag_given_codon3(state, seq, codon);
        break;
    case 4:
        lprob = lprob_frag_given_codon4(state, seq, codon);
        break;
    case 5:
        lprob = lprob_frag_given_codon5(state, seq, codon);
        break;
    default:
        return imm_lprob_nan();
    }

    return lprob + imm_codon_marg_lprob(state->codonm, *codon);
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
