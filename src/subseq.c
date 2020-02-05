#include "bug.h"
#include "subseq_static.h"

struct imm_subseq imm_subseq_init(struct imm_subseq* subseq, struct imm_seq const* seq,
                                  unsigned start, unsigned length)
{
    subseq->seq = seq;
    BUG(start + length > subseq->seq->length);
    subseq->subseq.abc = seq->abc;
    subseq->subseq.string = subseq->seq->string + start;
    subseq->subseq.length = length;
    return *subseq;
}

struct imm_seq const* imm_subseq_cast(struct imm_subseq const* subseq)
{
    return subseq_cast(subseq);
}

void imm_subseq_set(struct imm_subseq* subseq, unsigned start, unsigned length)
{
    BUG(start + length > subseq->seq->length);
    subseq->subseq.string = subseq->seq->string + start;
    subseq->subseq.length = length;
}
