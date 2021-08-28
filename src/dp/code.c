#include "dp/code.h"
#include "imm/seq.h"
#include "imm/state_types.h"
#include "matrix/matrix.h"
#include "support.h"
#include "xmem.h"
#include <limits.h>

static inline unsigned calc_size(struct imm_dp_code const *code)
{
    unsigned long ncombs = ipow(imm_abc_size(code->abc), code->seqlen.max);
    BUG(ncombs > UINT16_MAX);
    return (unsigned)(code->offset[code->seqlen.max - code->seqlen.min] +
                      ncombs);
}

void code_del(struct imm_dp_code const *code)
{
    free(code->offset);
    free(code->stride);
}

unsigned code_encode(struct imm_dp_code const *code, struct imm_seq const *seq)
{
    unsigned c = code->offset[imm_seq_size(seq) - code->seqlen.min];
    unsigned len = imm_seq_size(seq);
    for (unsigned i = 0; i < len; ++i)
    {
        unsigned j = imm_abc_symbol_idx(code->abc, seq->str[i]);
        unsigned offset = code->seqlen.max - len;
        c += code->stride[i + offset] * j;
    }
    return c;
}

void code_init(struct imm_dp_code *code, struct imm_abc const *abc)
{
    code->seqlen.min = IMM_STATE_NULL_SEQLEN;
    code->seqlen.max = IMM_STATE_NULL_SEQLEN;
    code->offset = NULL;
    code->stride = NULL;
    code->size = UINT_MAX;
    code->abc = abc;
}

void code_reset(struct imm_dp_code *code, unsigned min_seq, unsigned max_seq)
{
    BUG(!code->abc);
    if (code->seqlen.min == min_seq && code->seqlen.max == max_seq)
        return;

    code->seqlen.min = min_seq;
    code->seqlen.max = max_seq;

    code->stride =
        xrealloc(code->stride, sizeof(*code->stride) * code_stride_size(code));

    if (code_stride_size(code) > 0)
        code->stride[max_seq - 1] = 1;

    if (max_seq > 1)
    {
        for (unsigned len = max_seq - 2; 1 <= len + 1; --len)
            code->stride[len] =
                (uint16_t)(code->stride[len + 1] * imm_abc_size(code->abc));
    }

    code->offset =
        xrealloc(code->offset, sizeof(*code->offset) * code_offset_size(code));
    code->offset[0] = 0;
    for (unsigned len = min_seq + 1; len <= max_seq; ++len)
    {

        unsigned i = len - min_seq;
        code->offset[i] = (uint16_t)(code->offset[i - 1] +
                                     code->stride[max_seq - (len - 1) - 1]);
    }
    code->size = calc_size(code);
}
