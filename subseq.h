#ifndef IMM_SUBSEQ_H
#define IMM_SUBSEQ_H

#include "api.h"
#include "seq.h"

IMM_API struct imm_seq imm_subseq(struct imm_seq const *seq, unsigned start,
                                  unsigned size);
IMM_API void imm_subseq_init(struct imm_seq *subseq, struct imm_seq const *seq,
                             unsigned start, unsigned size);

#endif
