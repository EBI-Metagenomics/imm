#include "imm/subseq.h"
#include "imm/bug.h"

struct imm_subseq imm_subseq_init(struct imm_subseq* subseq, struct imm_seq const* seq,
                                  unsigned start, unsigned length)
{
    subseq->seq = seq;
    IMM_BUG(start + length > subseq->seq->length);
    subseq->subseq.abc = seq->abc;
    subseq->subseq.string = subseq->seq->string + start;
    subseq->subseq.length = length;
    return *subseq;
}
