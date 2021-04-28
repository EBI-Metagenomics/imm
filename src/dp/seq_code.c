#include "dp/seq_code.h"
#include "common/common.h"
#include "dp/eseq.h"
#include "imm/abc.h"
#include "imm/seq.h"
#include "matrix/matrix.h"

static inline unsigned calc_size(struct seq_code const *seq_code)
{
    unsigned long ncombs =
        ipow(imm_abc_len(seq_code->abc), seq_code->seqlen.max);
    BUG(ncombs > UINT16_MAX);
    return (unsigned)(seq_code->offset[seq_code->seqlen.max -
                                       seq_code->seqlen.min] +
                      ncombs);
}

static inline unsigned offset_size(struct seq_code const *seq_code)
{
    return (unsigned)(seq_code->seqlen.max - seq_code->seqlen.min + 1);
}

static inline unsigned stride_size(struct seq_code const *seq_code)
{
    return seq_code->seqlen.max;
}

unsigned seq_code_encode(struct seq_code const *seq_code,
                         struct imm_seq const *seq)
{
    unsigned code = seq_code->offset[imm_seq_len(seq) - seq_code->seqlen.min];
    unsigned len = imm_seq_len(seq);
    for (unsigned i = 0; i < len; ++i)
    {
        unsigned j = imm_abc_symbol_idx(seq_code->abc, imm_seq_str(seq)[i]);
        unsigned offset = seq_code->seqlen.max - len;
        code += seq_code->stride[i + offset] * j;
    }

    return code;
}

struct eseq *seq_code_new_eseq(struct seq_code const *seq_code)
{
    struct eseq *eseq = xmalloc(sizeof(*eseq));
    eseq->seq_code = seq_code;
    matrixu16_init(&eseq->code, 1,
                   seq_code->seqlen.max - seq_code->seqlen.min + 1);
    return eseq;
}

struct seq_code *seq_code_new(struct imm_abc const *abc, unsigned min_seq,
                              unsigned max_seq)
{
    BUG(min_seq > max_seq);
    struct seq_code *seq_code = xmalloc(sizeof(*seq_code));

    seq_code->seqlen.min = min_seq;
    seq_code->seqlen.max = max_seq;
    seq_code->abc = abc;

    if (max_seq == 0)
        seq_code->stride = NULL;
    else
    {
        seq_code->stride =
            xmalloc(sizeof(*seq_code->stride) * stride_size(seq_code));
        seq_code->stride[max_seq - 1] = 1;
    }

    if (max_seq > 1)
    {
        for (unsigned len = max_seq - 2; 1 <= len + 1; --len)
            seq_code->stride[len] =
                (uint16_t)(seq_code->stride[len + 1] * imm_abc_len(abc));
    }

    seq_code->offset =
        xmalloc(sizeof(*seq_code->offset) * offset_size(seq_code));
    seq_code->offset[0] = 0;
    for (unsigned len = min_seq + 1; len <= max_seq; ++len)
    {

        unsigned i = len - min_seq;
        seq_code->offset[i] =
            (uint16_t)(seq_code->offset[i - 1] +
                       seq_code->stride[max_seq - (len - 1) - 1]);
    }

    seq_code->size = calc_size(seq_code);

    return seq_code;
}

struct seq_code *seq_code_reset(struct seq_code *seq_code,
                                struct imm_abc const *abc, unsigned min_seq,
                                unsigned max_seq)
{

    if (seq_code->seqlen.min == min_seq && seq_code->seqlen.max == max_seq)
    {
        if (seq_code->abc == abc)
            return seq_code;
        seq_code->size = calc_size(seq_code);
        return seq_code;
    }
    seq_code_del(seq_code);
    return seq_code_new(abc, min_seq, max_seq);
}
