#ifndef IMM_SUBSEQ_H
#define IMM_SUBSEQ_H

#include "bug.h"
#include "imm/seq.h"

struct subseq
{
    struct imm_seq const* seq;
    struct imm_seq        subseq;
};

static inline struct subseq subseq_init(struct subseq* subseq, struct imm_seq const* seq,
                                        unsigned start, unsigned length)
{
    subseq->seq = seq;
    BUG(start + length > subseq->seq->length);
    subseq->subseq.abc = seq->abc;
    subseq->subseq.string = subseq->seq->string + start;
    subseq->subseq.length = length;
    return *subseq;
}

#define SUBSEQ(name, seq, start, length)                                                      \
    struct subseq name = subseq_init(&(name), seq, start, length)

static inline struct imm_seq const* subseq_cast(struct subseq const* subseq)
{
    return &subseq->subseq;
}

static inline void subseq_set(struct subseq* subseq, unsigned start, unsigned length)
{
    BUG(start + length > subseq->seq->length);
    subseq->subseq.string = subseq->seq->string + start;
    subseq->subseq.length = length;
}

#endif
