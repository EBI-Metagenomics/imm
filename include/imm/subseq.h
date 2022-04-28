#ifndef IMM_SUBSEQ_H
#define IMM_SUBSEQ_H

#include "imm/seq.h"

static inline struct imm_seq imm_subseq(struct imm_seq const *seq,
                                        unsigned start, unsigned size)
{
    return (struct imm_seq){(size), (seq)->str + (start), seq->abc};
}

static inline void imm_subseq_init(struct imm_seq *subseq,
                                   struct imm_seq const *seq, unsigned start,
                                   unsigned size)
{
    subseq->size = size;
    subseq->str = seq->str + start;
    subseq->abc = seq->abc;
}

#endif
