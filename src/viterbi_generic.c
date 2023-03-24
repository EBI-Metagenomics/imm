#include "viterbi_generic.h"
#include "imm/lprob.h"
#include "imm/viterbi.h"
#include "minmax.h"
#include "span.h"

void viterbi_generic(struct imm_viterbi const *x, struct imm_range const *range)
{
    for (unsigned row = range->a; row < range->b; ++row)
    {
        for (unsigned dst = 0; dst < imm_viterbi_nstates(x); ++dst)
        {
            unsigned remain = range->b + 1 - row;

            imm_float score = imm_lprob_zero();
            for (unsigned t = 0; t < imm_viterbi_ntrans(x, dst); ++t)
            {
                unsigned src = imm_viterbi_source(x, dst, t);
                struct imm_range src_range = imm_viterbi_range(x, src);
                src_range.b = MIN(src_range.b, row + 1);

                for (unsigned len = src_range.a; len < src_range.b; ++len)
                {
                    imm_float v0 = imm_viterbi_get_score(x, row, src, len);
                    imm_float v1 = imm_viterbi_trans_score(x, dst, t);
                    score = MAX(score, v0 + v1);
                }
            }

            struct imm_range dst_range = imm_viterbi_range(x, dst);
            dst_range.b = MIN(dst_range.b, remain + 1);

            if (row == 0 && imm_viterbi_start_state(x) == dst)
                score = MAX(imm_viterbi_start_lprob(x), score);

            for (unsigned len = dst_range.a; len < dst_range.b; ++len)
            {
                unsigned min = dst_range.a;
                imm_float v = imm_viterbi_emission(x, row, dst, len, min);
                imm_float total = score + v;
                imm_viterbi_set_score(x, imm_cell_init(row, dst, len), total);
            }
        }
    }
}
