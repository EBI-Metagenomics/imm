#include "eseq.h"
#include "free.h"
#include "imm/seq.h"
#include "imm/subseq.h"

void eseq_setup(struct eseq* eseq, struct imm_seq const* seq)
{
    unsigned ncols = matrixu_ncols(eseq->code);
    matrixu_resize(eseq->code, imm_seq_length(seq) + 1, ncols);

    for (unsigned i = 0; i <= imm_seq_length(seq); ++i) {

        for (unsigned j = 0; j < ncols; ++j) {

            unsigned length = eseq->seq_code->min_seq + j;
            if (i + length > imm_seq_length(seq))
                continue;

            IMM_SUBSEQ(subseq, seq, i, length);
            unsigned min_seq = eseq->seq_code->min_seq;
            unsigned code = seq_code_encode(eseq->seq_code, imm_subseq_cast(&subseq));
            code -= seq_code_offset(eseq->seq_code, min_seq);

            matrixu_set(eseq->code, i, j, code);
        }
    }
}

void eseq_destroy(struct eseq const* eseq)
{
    matrixu_destroy(eseq->code);
    free_c(eseq);
}
