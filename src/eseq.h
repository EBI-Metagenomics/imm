#ifndef IMM_ESEQ_H
#define IMM_ESEQ_H

#include "matrixu.h"
#include "seq_code.h"

struct eseq
{
    struct matrixu*        code;
    struct seq_code const* seq_code;
};

static inline unsigned eseq_get(struct eseq const* eseq, unsigned position, unsigned length)
{
    return matrixu_get(eseq->code, position, length - seq_code_min_seq(eseq->seq_code));
}
static inline unsigned eseq_length(struct eseq const* eseq)
{
    return matrixu_nrows(eseq->code) - 1;
}
IMM_EXPORT void eseq_destroy(struct eseq const* eseq);

#endif
