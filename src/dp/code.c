#include "dp/code.h"
#include "error.h"
#include "imm/seq.h"
#include "imm/state_types.h"
#include "ipow.h"
#include "matrix/matrix.h"
#include <limits.h>
#include <stdlib.h>

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

enum imm_rc code_reset(struct imm_dp_code *c, unsigned min_seq,
                       unsigned max_seq)
{
    BUG(!code->abc);
    if (c->seqlen.min == min_seq && c->seqlen.max == max_seq)
        return IMM_SUCCESS;

    c->seqlen.min = min_seq;
    c->seqlen.max = max_seq;

    c->stride = realloc(c->stride, sizeof *c->stride * code_stride_size(c));
    if (!c->stride && code_stride_size(c) > 0)
        return error(IMM_OUTOFMEM, "failed to realloc");

    if (code_stride_size(c) > 0)
        c->stride[max_seq - 1] = 1;

    if (max_seq > 1)
    {
        for (unsigned len = max_seq - 2; 1 <= len + 1; --len)
            c->stride[len] =
                (uint16_t)(c->stride[len + 1] * imm_abc_size(c->abc));
    }

    c->offset = realloc(c->offset, sizeof *c->offset * code_offset_size(c));
    if (!c->offset && code_offset_size(c) > 0)
    {
        free(c->stride);
        c->stride = NULL;
        return error(IMM_OUTOFMEM, "failed to realloc");
    }

    c->offset[0] = 0;
    for (unsigned len = min_seq + 1; len <= max_seq; ++len)
    {

        unsigned i = len - min_seq;
        c->offset[i] =
            (uint16_t)(c->offset[i - 1] + c->stride[max_seq - (len - 1) - 1]);
    }
    c->size = calc_size(c);
    return IMM_SUCCESS;
}
