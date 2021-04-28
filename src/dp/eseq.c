#include "dp/eseq.h"
#include "common/common.h"
#include "dp/seq_code.h"
#include "imm/seq.h"
#include "imm/subseq.h"

void eseq_del(struct eseq const *eseq)
{
    matrixu16_deinit(&eseq->code);
    free((void *)eseq);
}

struct eseq *eseq_new(struct seq_code const *seq_code, unsigned min_seq,
                      unsigned max_seq)
{
    struct eseq *eseq = xmalloc(sizeof(*eseq));
    eseq->seq_code = seq_code;
    matrixu16_init(&eseq->code, 1, max_seq - min_seq + 1);
    eseq->min_seq = min_seq;
    return eseq;
}

int eseq_setup(struct eseq *eseq, struct imm_seq const *seq)
{
    unsigned ncols = matrixu16_ncols(&eseq->code);
    matrixu16_resize(&eseq->code, imm_seq_len(seq) + 1, ncols);

    for (unsigned i = 0; i <= imm_seq_len(seq); ++i)
    {
        for (unsigned j = 0; j < ncols; ++j)
        {
            unsigned len = eseq->seq_code->seqlen.min + j;
            if (i + len > imm_seq_len(seq))
                continue;

            struct imm_seq subseq = IMM_SUBSEQ(seq, i, len);
            unsigned min_seq = eseq->seq_code->seqlen.min;
            unsigned code = seq_code_encode(eseq->seq_code, &subseq);
            code -= seq_code_offset(eseq->seq_code, min_seq);

            matrixu16_set(&eseq->code, i, j, (uint16_t)code);
        }
    }
    return IMM_SUCCESS;
}
