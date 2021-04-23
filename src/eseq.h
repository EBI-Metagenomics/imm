#ifndef ESEQ_H
#define ESEQ_H

#include "matrix.h"
#include "seq_code.h"

struct eseq
{
    struct matrixu16 *code;
    struct seq_code const *seq_code;
};

static inline void eseq_del(struct eseq const *eseq)
{
    matrixu16_destroy(eseq->code);
    free((void *)eseq);
}

static inline uint_fast16_t eseq_get(struct eseq const *eseq, uint_fast32_t pos,
                                     uint_fast32_t length)
{
    return matrixu16_get(eseq->code, pos, length - eseq->seq_code->min_seq);
}

static inline uint_fast32_t eseq_len(struct eseq const *eseq)
{
    return matrixu16_nrows(eseq->code) - 1;
}

int eseq_setup(struct eseq *eseq, struct imm_seq const *seq);

#endif
