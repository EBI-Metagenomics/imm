#include "dp/eseq.h"
#include "dp/code.h"
#include "imm/seq.h"
#include "imm/subseq.h"

void eseq_deinit(struct eseq const *eseq) { matrixu16_deinit(&eseq->data); }

void eseq_init(struct eseq *eseq, struct code const *code)
{
    eseq->code = code;
    matrixu16_init(&eseq->data, 1, code->seqlen.max - code->seqlen.min + 1);
}

void eseq_setup(struct eseq *eseq, struct imm_seq const *seq)
{
    unsigned ncols = matrixu16_ncols(&eseq->data);
    matrixu16_resize(&eseq->data, imm_seq_size(seq) + 1, ncols);

    for (unsigned i = 0; i <= imm_seq_size(seq); ++i)
    {
        for (unsigned j = 0; j < ncols; ++j)
        {
            unsigned len = eseq->code->seqlen.min + j;
            if (i + len > imm_seq_size(seq))
                continue;

            struct imm_seq subseq = imm_subseq(seq, i, len);
            unsigned min_seq = eseq->code->seqlen.min;
            unsigned code = code_encode(eseq->code, &subseq);
            code -= code_offset(eseq->code, min_seq);

            matrixu16_set(&eseq->data, i, j, (uint16_t)code);
        }
    }
}
