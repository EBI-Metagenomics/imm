#ifndef ESEQ_H
#define ESEQ_H

#include "matrixu.h"
#include "seq_code.h"

struct eseq
{
    struct matrixu*        code;
    struct seq_code const* seq_code;
};

void                   eseq_setup(struct eseq const* eseq, struct imm_seq const* seq);
static inline unsigned eseq_get(struct eseq const* eseq, unsigned position, unsigned length)
{
    return matrixu_get(eseq->code, position, length - seq_code_min_seq(eseq->seq_code));
}
static inline unsigned eseq_length(struct eseq const* eseq)
{
    return matrixu_nrows(eseq->code) - 1;
}
/* TODO: remove IMM_EXPORT */
IMM_EXPORT void eseq_destroy(struct eseq const* eseq);

#endif
