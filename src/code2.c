#include "code2.h"
#include "imm/code.h"
#include "imm/seq.h"
#include "ipow.h"
#include "macros.h"

static void calc_strides(uint16_t *stride, unsigned min_seq, unsigned max_seq,
                         unsigned abc_size)
{
    int last = (int)(max_seq - 1);
    stride[last] = 1;
    while (last > 0)
    {
        --last;
        stride[last] = (uint16_t)(stride[last + 1] * abc_size);
    }
}

static void calc_offsets(uint16_t *offset, unsigned min_seq, unsigned max_seq,
                         unsigned abc_size)
{
    unsigned n = abc_size;
    int i = 0;
    offset[i] = 0;
    /* +1 for sentinel */
    while (i < (int)(max_seq - min_seq + 1))
    {
        ++i;
        offset[i] = (uint16_t)(offset[i - 1] * n + u16pow(n, min_seq));
    }
}

unsigned code2_encode(struct imm_code const *code, struct imm_seq const *seq)
{
    unsigned len = imm_seq_size(seq);
    unsigned c = code->offset[len - code->seqlen.min];
    for (unsigned i = 0; i < len; ++i)
    {
        unsigned j = imm_seq_symbol_idx(seq, i);
        c += code->stride[code->seqlen.max - len + i] * j;
    }
    return c;
}

void code2_init(struct imm_code *code, struct imm_abc const *abc)
{
    code->seqlen.min = IMM_STATE_NULL_SEQLEN;
    code->seqlen.max = IMM_STATE_NULL_SEQLEN;
    memset(code->offset, 0, MEMBER_SIZE(*code, offset));
    memset(code->stride, 0, MEMBER_SIZE(*code, stride));
    code->abc = abc;
}

void code2_reset(struct imm_code *code, unsigned min_seq, unsigned max_seq)
{
    if (code->seqlen.min == min_seq && code->seqlen.max == max_seq) return;

    code->seqlen.min = min_seq;
    code->seqlen.max = max_seq;
    memset(code->offset, 0, MEMBER_SIZE(*code, offset));
    memset(code->stride, 0, MEMBER_SIZE(*code, stride));

    calc_strides(code->stride, min_seq, max_seq, imm_abc_size(code->abc));
    calc_offsets(code->offset, min_seq, max_seq, imm_abc_size(code->abc));
}
