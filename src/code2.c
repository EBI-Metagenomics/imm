#include "code2.h"
#include "imm/code.h"
#include "imm/seq.h"

static void calc_strides(uint16_t *stride, unsigned abc_size)
{
    int last = (int)(IMM_STATE_MAX_SEQLEN - 1);
    stride[last] = 1;
    while (last > 0)
    {
        --last;
        stride[last] = (uint16_t)(stride[last + 1] * abc_size);
    }
}

static void calc_offsets(uint16_t *offset, unsigned abc_size)
{
    unsigned n = abc_size;
    int i = 0;
    offset[i] = 0;
    /* +1 for sentinel */
    while (i < (int)(IMM_STATE_MAX_SEQLEN + 1))
    {
        ++i;
        offset[i] = (uint16_t)(offset[i - 1] * n + 1);
    }
}

unsigned code2_encode(struct imm_code const *code, struct imm_seq const *seq,
                      unsigned min_seq)
{
    unsigned len = imm_seq_size(seq);
    unsigned c = code->offset[len - min_seq];
    for (unsigned i = 0; i < len; ++i)
    {
        unsigned j = imm_seq_symbol_idx(seq, i);
        c += code->stride[IMM_STATE_MAX_SEQLEN - len + i] * j;
    }
    return c;
}

void code2_init(struct imm_code *code, struct imm_abc const *abc)
{
    calc_strides(code->stride, imm_abc_size(code->abc));
    calc_offsets(code->offset, imm_abc_size(code->abc));
    code->abc = abc;
}
