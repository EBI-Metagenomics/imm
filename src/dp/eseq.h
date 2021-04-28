#ifndef DP_ESEQ_H
#define DP_ESEQ_H

#include "matrix/matrix.h"

struct imm_seq;
struct seq_code;

struct eseq
{
    struct matrixu16 code;
    struct seq_code const *seq_code;
    unsigned min_seq;
};

void eseq_del(struct eseq const *eseq);

static inline unsigned eseq_get(struct eseq const *eseq, unsigned pos,
                                unsigned len)
{
    return matrixu16_get(&eseq->code, pos, len - eseq->min_seq);
}

static inline unsigned eseq_len(struct eseq const *eseq)
{
    return matrixu16_nrows(&eseq->code) - 1;
}

struct eseq *eseq_new(struct seq_code const *seq_code, unsigned min_seq,
                      unsigned max_seq);

int eseq_setup(struct eseq *eseq, struct imm_seq const *seq);

#endif
