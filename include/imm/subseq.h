#ifndef IMM_SUBSEQ_H
#define IMM_SUBSEQ_H

#include "imm/seq.h"

#define IMM_SUBSEQ(seq, start, len)                                            \
    (struct imm_seq) { (len), (seq)->str + (start), seq->abc, }

static inline void imm_subseq_init(struct imm_seq *subseq,
                                   struct imm_seq const *seq, unsigned start,
                                   unsigned len)
{
    subseq->len = len;
    subseq->str = seq->str + start;
    subseq->abc = seq->abc;
}

#endif
