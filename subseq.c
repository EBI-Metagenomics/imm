#include "subseq.h"
#include "range.h"

struct imm_seq imm_subseq(struct imm_seq const *seq, unsigned start,
                          unsigned size)
{
  imm_range(start, start + size);
  struct imm_str str = imm_str_slice(seq->str, imm_range(start, start + size));
  return (struct imm_seq){str, seq->abc};
}

void imm_subseq_init(struct imm_seq *subseq, struct imm_seq const *seq,
                     unsigned start, unsigned size)
{
  subseq->str.size = size;
  subseq->str.data = seq->str.data + start;
  subseq->abc = seq->abc;
}
