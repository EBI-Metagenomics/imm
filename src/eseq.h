#ifndef ESEQ_H
#define ESEQ_H

#include "matrix.h"
#include "seq_code.h"

struct eseq
{
    struct matrixu16*      code;
    struct seq_code const* seq_code;
};

void                        eseq_destroy(struct eseq const* eseq);
static inline uint_fast16_t eseq_get(struct eseq const* eseq, uint_fast16_t position,
                                     uint_fast16_t length);
static inline uint_fast16_t eseq_length(struct eseq const* eseq);
void                        eseq_setup(struct eseq* eseq, struct imm_seq const* seq);

static inline uint_fast16_t eseq_get(struct eseq const* eseq, uint_fast16_t position,
                                     uint_fast16_t length)
{
    return matrixu16_get(eseq->code, position, length - seq_code_min_seq(eseq->seq_code));
}

static inline uint_fast16_t eseq_length(struct eseq const* eseq)
{
    return (uint_fast16_t)(matrixu16_nrows(eseq->code) - 1);
}

#endif
