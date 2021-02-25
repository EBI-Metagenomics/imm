#ifndef SUBSEQ_H
#define SUBSEQ_H

#include "imm/imm.h"
#include <stdint.h>

#define SUBSEQ(SEQ, START, LENGTH)                                                                                     \
    {                                                                                                                  \
        .abc = SEQ->abc, .string = SEQ->string + (START), .length = (LENGTH)                                           \
    }

static inline void subseq_set(struct imm_seq* subseq, struct imm_seq const* seq, uint32_t start, uint32_t length)
{
    subseq->string = seq->string + start;
    subseq->length = length;
}

#endif
