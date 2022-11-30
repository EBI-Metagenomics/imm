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

// Let S be the random variable of sequences obeying our codon-hmm-tree schema.
// The lengths of S vary from 1 to 5: len(S) \in {1, 2, 3, 4, 5}
// Let N = len(S).
//
// Let f(s0,s1,s2) be the probability of a given state (pre-modification)
// emitting the codon (s0, s1, s2). Let the symbol _ denote marginalisation
// over the corresponding random variable.
//
// Let
//
//     T \in {Nop, Del}x{Nop, Del}x{Nop, Ins}x{Nop, Ins}
//
// be the random variable of paths in the codon-hmm-tree.
//
// Notice that p(N|S) \in {0, 1}:
//
//     p(N|S) = 1 if N=len(S)
//     p(N|S) = 0 otherwise

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
    (struct imm_codon)                                                         \
    {                                                                          \
        .a = (A), .b = (B), .c = (C)                                           \
    }

#define lf(A, B, C)                                                            \
    imm_codon_marg_lprob(state->codonm, CODON(nucl[A], nucl[B], nucl[C]))

static inline imm_float ln(imm_float x) { return imm_log(x); }

// p(S=(s0,))
//     = p(S=(s0,)|N=1) * p(N=1)
//     = p(S=(s0,)|T=ddnn) * p(T=ddnn)
//
// p(S=(s0,)|T=ddnn) = (f(0,_,_) + f(_,0,_) + f(_,_,0)) / 3
//
// p(T=ddnn) = e^2 * (1-e)^2
static imm_float joint_seq_len1(struct imm_frame_state const *state,
                                struct imm_seq const *seq)
{
    struct imm_abc const *abc = imm_super(state->codonm->nuclt);
    unsigned nucl[2] = {imm_abc_symbol_idx(abc, imm_seq_str(seq)[0]),
                        imm_abc_any_symbol_idx(abc)};
    unsigned _ = IMM_ARRAY_SIZE(nucl) - 1;

    // exp(c) = p(N=1) = p(T=ddnn)
    imm_float c = 2 * state->eps.loge + 2 * state->eps.log1e;

    // exp(v) = p(S=(s0,)|N=1) = p(S=(s0,)|T=ddnn)
    imm_float p = logsum(lf(0, _, _), lf(_, 0, _), lf(_, _, 0)) - ln(3);

    // ln(p(S=(s0,)))
    return c + p;
}

// p(S=(s0,s1))
//     = p(S=(s0,s1)|N=2) * p(N=2)
//     = p(S=(s0,s1)|T=ndnn) * p(T=ndnn) + p(S=(s0,s1)|T=dnnn) * p(T=dnnn)
//     + p(S=(s0,s1)|T=ddni) * p(T=ddni) + p(S=(s0,s1)|T=ddin) * p(T=ddin)
//
// p(S=(s0,s1)|T=ndnn) = p(S=(s0,s1)|T=dnnn)
//     = (f(s0,s1,_) + f(s0,_,s1) + f(_,s0,s1)) / 3
//
// p(S=(s0,s1)|T=ddni) = p(S=(s0,s1)|T=ddin)
//                     = (f(s0,_,_) * i(s1)
//                     +  f(_,s0,_) * i(s1)
//                     +  f(_,_,s0) * i(s1)
//                     +  f(s1,_,_) * i(s0)
//                     +  f(_,s1,_) * i(s0)
//                     +  f(_,_,s1) * i(s0)) / 6
//
// p(T=ndnn) = p(T=dnnn) = e^1 * (1-e)^3
// p(T=ddni) = p(T=ddin) = e^3 * (1-e)^1
static imm_float joint_seq_len2(struct imm_frame_state const *state,
                                struct imm_seq const *seq)
{
    struct imm_abc const *abc = imm_super(state->codonm->nuclt);
    unsigned nucl[3] = {imm_abc_symbol_idx(abc, imm_seq_str(seq)[0]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[1]),
                        imm_abc_any_symbol_idx(abc)};
    unsigned _ = IMM_ARRAY_SIZE(nucl) - 1;

    imm_float i0 = __imm_nuclt_lprob_get(state->nucltp, nucl[0]);
    imm_float i1 = __imm_nuclt_lprob_get(state->nucltp, nucl[1]);

    // exp(c0) = p(T=ndnn) = p(T=dnnn)
    imm_float c0 = state->eps.loge + 3 * state->eps.log1e;
    // exp(c1) = p(T=ddni) = p(T=ddin)
    imm_float c1 = 3 * state->eps.loge + state->eps.log1e;

    // exp(p0) = p(S=(s0,s1)|T=ndnn) = p(S=(s0,s1)|T=dnnn)
    imm_float p0 = logsum(lf(0, 1, _), lf(0, _, 1), lf(_, 0, 1)) - ln(3);
    // exp(p1) = p(S=(s0,s1)|T=ddni) = p(S=(s0,s1)|T=ddin)
    imm_float p1 = logsum(logsum(lf(0, _, _), lf(_, 0, _), lf(_, _, 0)) + i1,
                          logsum(lf(1, _, _), lf(_, 1, _), lf(_, _, 1)) + i0) -
                   ln(6);

    // ln(p(S=(s0,s1)))
    return logsum(ln(2) + p0 + c0, ln(2) + p1 + c1);
}

// p(S=(s0,s1,s2))
//     = p(S=(s0,s1,s2)|N=3) * p(N=3)
//     = p(S=(s0,s1,s2)|T=nnnn) * p(T=nnnn)
//     + p(S=(s0,s1,s2)|T=dnin) * p(T=dnin)
//     + p(S=(s0,s1,s2)|T=dnni) * p(T=dnni)
//     + p(S=(s0,s1,s2)|T=ndin) * p(T=ndin)
//     + p(S=(s0,s1,s2)|T=ndni) * p(T=ndni)
//     + p(S=(s0,s1,s2)|T=ddii) * p(T=ddii)
//
// p(S=(s0,s1,s2)|T=nnnn) = f(s0, s1, s2)
// p(S=(s0,s1,s2)|T=dnin)
//     = p(S=(s0,s1,s2)|T=dnni)
//     = p(S=(s0,s1,s2)|T=ndin)
//     = p(S=(s0,s1,s2)|T=ndni)
//     = f(0,1,_) * i(2) + f(0,_,1) * i(2) + f(_,0,1) * i(2)
//     + f(0,2,_) * i(1) + f(0,_,2) * i(1) + f(_,0,2) * i(1)
//     + f(1,2,_) * i(0) + f(1,_,2) * i(0) + f(_,1,2) * i(0)
// p(S=(s0,s1,s2)|T=ddii)
//     = f(0,_,_)*i(1)*i(2) + f(_,0,_)*i(1)*i(2) + f(_,_,0)*i(1)*i(2)
//     + f(1,_,_)*i(0)*i(2) + f(_,1,_)*i(0)*i(2) + f(_,_,1)*i(0)*i(2)
//     + f(2,_,_)*i(0)*i(1) + f(_,2,_)*i(0)*i(1) + f(_,_,2)*i(0)*i(1)
//
// p(T=nnnn) = e^0 * (1-e)^4
// p(T=dnin) = p(T=dnni) = p(T=ndin) = p(T=ndni)
//           = e^2 * (1-e)^2
// p(T=ddii) = e^4 * (1-e)^0
static imm_float joint_seq_len3(struct imm_frame_state const *state,
                                struct imm_seq const *seq)
{
    struct imm_abc const *abc = imm_super(state->codonm->nuclt);
    unsigned nucl[4] = {imm_abc_symbol_idx(abc, imm_seq_str(seq)[0]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[1]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[2]),
                        imm_abc_any_symbol_idx(abc)};
    unsigned _ = IMM_ARRAY_SIZE(nucl) - 1;

    imm_float i0 = __imm_nuclt_lprob_get(state->nucltp, nucl[0]);
    imm_float i1 = __imm_nuclt_lprob_get(state->nucltp, nucl[1]);
    imm_float i2 = __imm_nuclt_lprob_get(state->nucltp, nucl[2]);

    // exp(c0) = p(T=nnnn)
    imm_float c0 = 0 * state->eps.loge + 4 * state->eps.log1e;
    // exp(c1) = p(T=dnin) = p(T=dnni) = p(T=ndin) = p(T=ndni)
    imm_float c1 = 2 * state->eps.loge + 2 * state->eps.log1e;
    // exp(c2) = p(T=ddii)
    imm_float c2 = 4 * state->eps.loge + 0 * state->eps.log1e;

    // exp(p0) = p(S=(s0,s1,s2)|T=nnnn)
    imm_float p0 = lf(0, 1, 2);
    // exp(p1) = p(S=(s0,s1,s2)|T=dnin) = p(S=(s0,s1,s2)|T=dnni)
    //         = p(S=(s0,s1,s2)|T=ndin) = p(S=(s0,s1,s2)|T=ndni)
    imm_float p1 = logsum(logsum(lf(0, 1, _), lf(0, _, 1), lf(_, 0, 1)) + i2,
                          logsum(lf(0, 2, _), lf(0, _, 2), lf(_, 0, 2)) + i1,
                          logsum(lf(1, 2, _), lf(1, _, 2), lf(_, 1, 2)) + i0) -
                   ln(9);
    // exp(p2) = p(S=(s0,s1,s2)|T=ddii)
    imm_float p2 =
        logsum(logsum(lf(0, _, _), lf(_, 0, _), lf(_, _, 0)) + i1 + i2,
               logsum(lf(1, _, _), lf(_, 1, _), lf(_, _, 1)) + i0 + i2,
               logsum(lf(2, _, _), lf(_, 2, _), lf(_, _, 2)) + i0 + i1) -
        ln(9);

    // ln(p(S=(s0,s1,s2)))
    return logsum(p0 + c0, ln(4) + p1 + c1, p2 + c2);
}

// p(S=(s0,s1,s2,s3))
//     = p(S=(s0,s1,s2,s3)|N=4) * p(N=4)
//     = p(S=(s0,s1,s2,s3)|T=nnin) * p(T=nnin)
//     + p(S=(s0,s1,s2,s3)|T=nnni) * p(T=nnni)
//     + p(S=(s0,s1,s2,s3)|T=dnii) * p(T=dnii)
//     + p(S=(s0,s1,s2,s3)|T=ndii) * p(T=ndii)
//
// p(S=(s0,s1,s2,s3)|T=nnin) = p(S=(s0,s1,s2,s3)|T=nnni)
//     = f(0,1,2) * i(3) + f(0,1,3) * i(2)
//     + f(0,2,3) * i(1) + f(1,2,3) * i(0)
//
// p(S=(s0,s1,s2,s3)|T=dnii) = p(S=(s0,s1,s2,s3)|T=ndii)
//     = (f(0,1,_)*i(2)*i(3) + f(0,_,1)*i(2)*i(3) + f(_,0,1)*i(2)*i(3)
//     +  f(0,2,_)*i(1)*i(3) + f(0,_,2)*i(1)*i(3) + f(_,0,2)*i(1)*i(3)
//     +  f(1,2,_)*i(0)*i(3) + f(1,_,2)*i(0)*i(3) + f(_,1,2)*i(0)*i(3)
//     +  f(0,3,_)*i(1)*i(2) + f(0,_,3)*i(1)*i(2) + f(_,0,3)*i(1)*i(2)
//     +  f(1,3,_)*i(0)*i(2) + f(1,_,3)*i(0)*i(2) + f(_,1,3)*i(0)*i(2)
//     +  f(2,3,_)*i(0)*i(1) + f(2,_,3)*i(0)*i(1) + f(_,2,3)*i(0)*i(1)) / 18
//
// p(T=nnin) = p(T=nnni) = e^1 * (1-e)^3
// p(T=dnii) = p(T=ndii) = e^3 * (1-e)^1
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

    imm_float i0 = __imm_nuclt_lprob_get(state->nucltp, nucl[0]);
    imm_float i1 = __imm_nuclt_lprob_get(state->nucltp, nucl[1]);
    imm_float i2 = __imm_nuclt_lprob_get(state->nucltp, nucl[2]);
    imm_float i3 = __imm_nuclt_lprob_get(state->nucltp, nucl[3]);

    // exp(c0) = p(T=nnin) = p(T=nnni) = e^1 * (1-e)^3
    imm_float c0 = state->eps.loge + 3 * state->eps.log1e;
    // exp(p0) = p(S=(s0,s1,s2,s3)|T=nnin) = p(S=(s0,s1,s2,s3)|T=nnni)
    imm_float p0 = logsum(lf(0, 1, 2) + i3, lf(0, 1, 3) + i2, lf(0, 2, 3) + i1,
                          lf(1, 2, 3) + i0);

    // exp(c1) = p(T=dnii) = p(T=ndii) = e^3 * (1-e)^1
    imm_float c1 = 3 * state->eps.loge + state->eps.log1e;
    // exp(p1) = p(S=(s0,s1,s2,s3)|T=dnii) = p(S=(s0,s1,s2,s3)|T=ndii)
    imm_float p1 = logsum(lf(0, 1, _) + i2 + i3, lf(0, _, 1) + i2 + i3,
                          lf(_, 0, 1) + i2 + i3, lf(0, 2, _) + i1 + i3,
                          lf(0, _, 2) + i1 + i3, lf(_, 0, 2) + i1 + i3,
                          lf(1, 2, _) + i0 + i3, lf(1, _, 2) + i0 + i3,
                          lf(_, 1, 2) + i0 + i3, lf(0, 3, _) + i1 + i2,
                          lf(0, _, 3) + i1 + i2, lf(_, 0, 3) + i1 + i2,
                          lf(1, 3, _) + i0 + i2, lf(1, _, 3) + i0 + i2,
                          lf(_, 1, 3) + i0 + i2, lf(2, 3, _) + i0 + i1,
                          lf(2, _, 3) + i0 + i1, lf(_, 2, 3) + i0 + i1) -
                   ln(9);

    return imm_lprob_add(c0 + p0 - ln(2), c1 + p1);
}

// p(S=(s0,s1,s2,s3,s4))
//     = p(S=(s0,s1,s2,s3,s4)|N=5) * p(N=5)
//     = p(S=(s0,s1,s2,s3,s4)|T=nnii) * p(T=nnii)
//
// p(S=(s0,s1,s2,s3,s4)|T=nnii) =
//     = (f(0, 1, 2) * i(3) * i(4) + f(0, 1, 3) * i(2) * i(4)
//     +  f(0, 2, 3) * i(1) * i(4) + f(1, 2, 3) * i(0) * i(4)
//     +  f(0, 1, 4) * i(2) * i(3) + f(0, 2, 4) * i(1) * i(3)
//     +  f(1, 2, 4) * i(0) * i(3) + f(0, 3, 4) * i(1) * i(2)
//     +  f(1, 3, 4) * i(0) * i(2) + f(2, 3, 4) * i(0) * i(1)) / 10
//
// p(T=nnii) = e^2 * (1-e)^2
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

    imm_float i0 = __imm_nuclt_lprob_get(state->nucltp, nucl[0]);
    imm_float i1 = __imm_nuclt_lprob_get(state->nucltp, nucl[1]);
    imm_float i2 = __imm_nuclt_lprob_get(state->nucltp, nucl[2]);
    imm_float i3 = __imm_nuclt_lprob_get(state->nucltp, nucl[3]);
    imm_float i4 = __imm_nuclt_lprob_get(state->nucltp, nucl[4]);

    // exp(c) = p(T=nnii)
    imm_float c = 2 * state->eps.loge + 2 * state->eps.log1e;

    // exp(p) = p(S=(s0,s1,s2,s3,s4)|T=nnii)
    imm_float p = logsum(logsum(i0 + i1 + lf(2, 3, 4), i0 + i2 + lf(1, 3, 4)),
                         logsum(i0 + i3 + lf(1, 2, 4), i0 + i4 + lf(1, 2, 3)),
                         logsum(i1 + i2 + lf(0, 3, 4), i1 + i3 + lf(0, 2, 4)),
                         logsum(i1 + i4 + lf(0, 2, 3), i2 + i3 + lf(0, 1, 4)),
                         logsum(i2 + i4 + lf(0, 1, 3), i3 + i4 + lf(0, 1, 2))) -
                  log(10);

    return c + p;
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
        error(IMM_SEQ_OUT_OF_RANGE);
    }

    return imm_lprob_zero();
}
