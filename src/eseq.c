#include "imm/eseq.h"
#include "code.h"
#include "error.h"
#include "imm/seq.h"
#include "imm/subseq.h"

void imm_eseq_del(struct imm_eseq const *eseq)
{
    imm_matrixu16_deinit(&eseq->data);
}

void imm_eseq_init(struct imm_eseq *eseq, struct imm_code const *code)
{
    imm_matrixu16_empty(&eseq->data);
    imm_eseq_reset(eseq, code);
}

void imm_eseq_reset(struct imm_eseq *eseq, struct imm_code const *code)
{
    eseq->code = code;
}

unsigned imm_eseq_get(struct imm_eseq const *eseq, unsigned pos, unsigned len,
                      unsigned min_seq)
{
    uint16_t val = imm_matrixu16_get(&eseq->data, pos, len);
    return code_translate(eseq->code, val, min_seq);
}

unsigned imm_eseq_len(struct imm_eseq const *eseq)
{
    return eseq->data.rows - 1;
}

int imm_eseq_setup(struct imm_eseq *eseq, struct imm_seq const *seq)
{
    unsigned ncols = IMM_STATE_MAX_SEQLEN + 1;
    int rc = 0;

    if ((rc = imm_matrixu16_resize(&eseq->data, imm_seq_size(seq) + 1, ncols)))
        return rc;

    for (unsigned i = 0; i <= imm_seq_size(seq); ++i)
    {
        for (unsigned len = 0; len < ncols; ++len)
        {
            if (i + len > imm_seq_size(seq)) break;

            struct imm_seq subseq = imm_subseq(seq, i, len);
            unsigned code = imm_code_encode(eseq->code, &subseq);
            imm_matrixu16_set(&eseq->data, i, len, (uint16_t)code);
        }
    }
    return rc;
}
