#ifndef DP_ESEQ_H
#define DP_ESEQ_H

#include "matrix/matrix.h"

struct imm_seq;
struct code;

struct eseq
{
    struct matrixu16 data;
    struct code const *code;
    unsigned min_seq;
};

void eseq_deinit(struct eseq const *eseq);

static inline unsigned eseq_get(struct eseq const *eseq, unsigned pos,
                                unsigned len)
{
    return matrixu16_get(&eseq->data, pos, len - eseq->min_seq);
}

static inline unsigned eseq_len(struct eseq const *eseq)
{
    return matrixu16_nrows(&eseq->data) - 1;
}

void eseq_init(struct eseq *eseq, struct code const *code);

void eseq_setup(struct eseq *eseq, struct imm_seq const *seq);

#endif
