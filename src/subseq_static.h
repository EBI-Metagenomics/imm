#ifndef IMM_SUBSEQ_STATIC_H
#define IMM_SUBSEQ_STATIC_H

#include "imm/subseq.h"

static inline struct imm_seq const* subseq_cast(struct imm_subseq const* subseq)
{
    return &subseq->subseq;
}

#endif
