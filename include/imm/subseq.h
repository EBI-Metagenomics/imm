#ifndef IMM_SUBSEQ_H
#define IMM_SUBSEQ_H

#include "imm/bug.h"
#include "imm/export.h"
#include "imm/seq.h"

struct imm_subseq
{
    struct imm_seq const* seq;
    struct imm_seq        subseq;
};

#define IMM_SUBSEQ(name, seq, start, length)                                                  \
    struct imm_subseq name = imm_subseq_init(&(name), seq, start, length)

IMM_EXPORT struct imm_subseq    imm_subseq_init(struct imm_subseq*    subseq,
                                                struct imm_seq const* seq, unsigned start,
                                                unsigned length);
static inline struct imm_subseq imm_subseq_slice(struct imm_seq const* seq, unsigned start,
                                                 unsigned length)
{
    IMM_SUBSEQ(subseq, seq, start, length);
    return subseq;
}
static inline struct imm_seq const* imm_subseq_cast(struct imm_subseq const* subseq)
{
    return &subseq->subseq;
}
static inline void imm_subseq_set(struct imm_subseq* subseq, unsigned start, unsigned length)
{
    IMM_BUG(start + length > subseq->seq->length);
    subseq->subseq.string = subseq->seq->string + start;
    subseq->subseq.length = length;
}
static inline unsigned imm_subseq_start(struct imm_subseq const* subseq)
{
    IMM_BUG(subseq->subseq.string < subseq->seq->string);
    return (unsigned)(subseq->subseq.string - subseq->seq->string);
}
static inline unsigned imm_subseq_length(struct imm_subseq const* subseq)
{
    return subseq->subseq.length;
}

#endif
