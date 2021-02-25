#ifndef IMM_SUBSEQ_H
#define IMM_SUBSEQ_H

#include "imm/seq.h"
#include <stdint.h>

#define IMM_SUBSEQ(SEQ, START, LENGTH)                                                                                 \
    {                                                                                                                  \
        .abc = SEQ->abc, .string = SEQ->string + (START), .length = (LENGTH)                                           \
    }

static inline void imm_subseq_set(struct imm_seq* subseq, struct imm_seq const* seq, uint32_t start, uint32_t length)
{
    subseq->string = seq->string + start;
    subseq->length = length;
}

#endif
