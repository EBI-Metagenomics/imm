#include "frame_state_24.h"
#include "imm/codon_marg.h"
#include "imm/frame_cond.h"
#include "imm/frame_state.h"
#include "imm/generics.h"
#include "imm/lprob.h"
#include "imm/nuclt_lprob.h"
#include "logsum.h"

#define codon(A, B, C)                                                         \
    (struct imm_codon)                                                         \
    {                                                                          \
        .a = (A), .b = (B), .c = (C)                                           \
    }

#define lf(A, B, C)                                                            \
    imm_codon_marg_lprob(state->codonm, codon(nucl[A], nucl[B], nucl[C]))

static inline imm_float ln(imm_float x) { return imm_log(x); }

// p(S=(s0,s1))
//     = p(S=(s0,s1)|N=2) * p(N=2)
//     = p(S=(s0,s1)|T=dn) * p(T=dn)
//
// p(S=(s0,s1)|T=dn) = (f(0,1,_) + f(0,_,1) + f(_,0,1)) / 3
//
// p(T=dn) = e^1 * (1-e)^1
imm_float imm_joint_n2_24(struct imm_frame_state const *state,
                          struct imm_seq const *seq)
{
    struct imm_abc const *abc = imm_super(state->codonm->nuclt);
    unsigned nucl[3] = {imm_abc_symbol_idx(abc, imm_seq_str(seq)[0]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[1]),
                        imm_abc_any_symbol_idx(abc)};
    unsigned _ = IMM_ARRAY_SIZE(nucl) - 1;

    // exp(c) = p(T=dn)
    imm_float c = 1 * state->eps.loge + 1 * state->eps.log1e;

    // exp(p) = p(S=(s0,s1)|T=dn)
    imm_float p = logsum(lf(0, 1, _), lf(0, _, 1), lf(_, 0, 1)) - ln(3);

    // ln(p(S=(s0,s1)))
    return p + c;
}

// p(S=(s0,s1,s2))
//     = p(S=(s0,s1,s2)|N=3) * p(N=3)
//     = p(S=(s0,s1,s2)|T=nn) * p(T=nn)
//     + p(S=(s0,s1,s2)|T=di) * p(T=di)
//
// p(S=(s0,s1,s2)|T=nn) = f(0,1,2)
// p(S=(s0,s1,s2)|T=di)
//     = (f(0,1,_) * i(2) + f(0,_,1) * i(2) + f(_,0,1) * i(2)
//     +  f(0,2,_) * i(1) + f(0,_,2) * i(1) + f(_,0,2) * i(1)
//     +  f(1,2,_) * i(0) + f(1,_,2) * i(0) + f(_,1,2) * i(0)) / 9
//
// p(T=nn) = e^0 * (1-e)^2
// p(T=di) = e^2 * (1-e)^0
imm_float imm_joint_n3_24(struct imm_frame_state const *state,
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

    // exp(c0) = p(T=nn)
    imm_float c0 = 0 * state->eps.loge + 2 * state->eps.log1e;

    // exp(c1) = p(T=di)
    imm_float c1 = 2 * state->eps.loge + 0 * state->eps.log1e;

    // exp(p0) = p(S=(s0,s1,s2)|T=nn)
    imm_float p0 = lf(0, 1, 2);

    // exp(p1) = p(S=(s0,s1,s2)|T=di)
    imm_float p1 =
        logsum(lf(0, 1, _) + i2, lf(0, _, 1) + i2, lf(_, 0, 1) + i2,
               lf(0, 2, _) + i1, lf(0, _, 2) + i1, lf(_, 0, 2) + i1,
               lf(1, 2, _) + i0, lf(1, _, 2) + i0, lf(_, 1, 2) + i0) -
        ln(9);

    // ln(p(S=(s0,s1,s2)))
    return logsum(p0 + c0, p1 + c1);
}

// p(S=(s0,s1,s2,s3))
//     = p(S=(s0,s1,s2,s3)|N=4) * p(N=4)
//     = p(S=(s0,s1,s2,s3)|T=ni) * p(T=ni)
//
// p(S=(s0,s1,s2,s3)|T=ni) = (f(0,1,2) * i(3) + f(0,1,3) * i(2)
//                         +  f(0,2,3) * i(1) + f(1,2,3) * i(0)) / 4
//
// p(T=ni) = e^1 * (1-e)^1
imm_float imm_joint_n4_24(struct imm_frame_state const *state,
                          struct imm_seq const *seq)
{
    struct imm_abc const *abc = imm_super(state->codonm->nuclt);
    unsigned nucl[5] = {imm_abc_symbol_idx(abc, imm_seq_str(seq)[0]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[1]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[2]),
                        imm_abc_symbol_idx(abc, imm_seq_str(seq)[3]),
                        imm_abc_any_symbol_idx(abc)};

    imm_float i0 = __imm_nuclt_lprob_get(state->nucltp, nucl[0]);
    imm_float i1 = __imm_nuclt_lprob_get(state->nucltp, nucl[1]);
    imm_float i2 = __imm_nuclt_lprob_get(state->nucltp, nucl[2]);
    imm_float i3 = __imm_nuclt_lprob_get(state->nucltp, nucl[3]);

    // exp(c) = p(T=ni)
    imm_float c = 1 * state->eps.loge + 1 * state->eps.log1e;

    // exp(p) = p(S=(s0,s1,s2,s3)|T=ni)
    imm_float p = logsum(lf(0, 1, 2) * i3, lf(0, 1, 3) * i2, lf(0, 2, 3) * i1,
                         lf(1, 2, 3) * i0) -
                  ln(4);

    // ln(p(S=(s0,s1,s2,s3)))
    return p + c;
}
