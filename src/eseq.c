#include "eseq.h"
#include "code.h"
#include "error.h"
#include "imm/seq.h"
#include "imm/subseq.h"

void imm_eseq_del(struct eseq const *eseq) { matrixu16_deinit(&eseq->data); }

void imm_eseq_init(struct eseq *eseq, struct imm_code const *code)
{
    matrixu16_empty(&eseq->data);
    imm_eseq_reset(eseq, code);
}

void imm_eseq_reset(struct eseq *eseq, struct imm_code const *code)
{
    eseq->code = code;
}

enum imm_rc imm_eseq_setup(struct eseq *eseq, struct imm_seq const *seq)
{
    unsigned ncols = IMM_STATE_MAX_SEQLEN + 1;
    if (matrixu16_resize(&eseq->data, imm_seq_size(seq) + 1, ncols))
        return error(IMM_NOMEM);

    for (unsigned i = 0; i <= imm_seq_size(seq); ++i)
    {
        for (unsigned len = 0; len < ncols; ++len)
        {
            if (i + len > imm_seq_size(seq)) break;

            struct imm_seq subseq = imm_subseq(seq, i, len);
            unsigned code = imm_code_encode(eseq->code, &subseq);
            matrixu16_set(&eseq->data, i, len, (uint16_t)code);
        }
    }
    return IMM_OK;
}
