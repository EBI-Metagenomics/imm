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
static inline uint_fast16_t eseq_get(struct eseq const* eseq, uint_fast32_t position, uint_fast32_t length);
static inline uint_fast32_t eseq_length(struct eseq const* eseq);
int                         eseq_setup(struct eseq* eseq, struct imm_seq const* seq);

static inline uint_fast16_t eseq_get(struct eseq const* eseq, uint_fast32_t position, uint_fast32_t length)
{
    return matrixu16_get(eseq->code, position, length - seq_code_min_seq(eseq->seq_code));
}

static inline uint_fast32_t eseq_length(struct eseq const* eseq) { return matrixu16_nrows(eseq->code) - 1; }

#endif
