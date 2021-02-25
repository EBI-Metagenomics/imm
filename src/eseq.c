#include "eseq.h"
#include "free.h"
#include "imm/seq.h"
#include "imm/subseq.h"

void eseq_destroy(struct eseq const* eseq)
{
    matrixu16_destroy(eseq->code);
    free_c(eseq);
}

void eseq_setup(struct eseq* eseq, struct imm_seq const* seq)
{
    uint_fast32_t ncols = matrixu16_ncols(eseq->code);
    matrixu16_resize(eseq->code, imm_seq_length(seq) + 1, ncols);

    for (uint_fast32_t i = 0; i <= imm_seq_length(seq); ++i) {

        for (uint_fast32_t j = 0; j < ncols; ++j) {

            uint_fast32_t length = eseq->seq_code->min_seq + j;
            if (i + length > imm_seq_length(seq))
                continue;

            IMM_SUBSEQ(subseq, seq, (uint32_t)i, (uint32_t)length);
            uint_fast8_t  min_seq = eseq->seq_code->min_seq;
            uint_fast16_t code = seq_code_encode(eseq->seq_code, imm_subseq_cast(&subseq));
            code -= seq_code_offset(eseq->seq_code, min_seq);

            matrixu16_set(eseq->code, i, j, (uint16_t)code);
        }
    }
}
