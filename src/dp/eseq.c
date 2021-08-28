#include "dp/eseq.h"
#include "dp/code.h"
#include "imm/seq.h"
#include "imm/subseq.h"
#include "log.h"

void eseq_del(struct eseq const *eseq) { matrixu16_deinit(&eseq->data); }

void eseq_init(struct eseq *eseq, struct imm_dp_code const *code)
{
    matrixu16_empty(&eseq->data);
    eseq_reset(eseq, code);
}

void eseq_reset(struct eseq *eseq, struct imm_dp_code const *code)
{
    eseq->code = code;
}

enum imm_rc eseq_setup(struct eseq *eseq, struct imm_seq const *seq)
{
    unsigned ncols = eseq->code->seqlen.max - eseq->code->seqlen.min + 1;
    if (matrixu16_resize(&eseq->data, imm_seq_size(seq) + 1, ncols))
        return error(IMM_OUTOFMEM, "failed to resize");

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
    return IMM_SUCCESS;
}
