#ifndef DP_ESEQ_H
#define DP_ESEQ_H

#include "dp/code.h"
#include "matrix/matrix.h"

struct imm_seq;
struct imm_dp_code;

struct eseq
{
    struct matrixu16 data;
    struct imm_dp_code const *code;
};

void eseq_del(struct eseq const *eseq);

void eseq_reset(struct eseq *eseq, struct imm_dp_code const *code);

static inline unsigned eseq_get(struct eseq const *eseq, unsigned pos,
                                unsigned len)
{
    return matrixu16_get(&eseq->data, pos, len - eseq->code->seqlen.min);
}

static inline unsigned eseq_len(struct eseq const *eseq)
{
    return matrixu16_nrows(&eseq->data) - 1;
}

void eseq_init(struct eseq *eseq, struct imm_dp_code const *code);

void eseq_setup(struct eseq *eseq, struct imm_seq const *seq);

#endif
