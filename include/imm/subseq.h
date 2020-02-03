#ifndef IMM_SUBSEQ_H
#define IMM_SUBSEQ_H

#include "imm/seq.h"

struct imm_subseq
{
    struct imm_seq const* seq;
    struct imm_seq        subseq;
};

struct imm_subseq imm_subseq_init(struct imm_subseq* subseq, struct imm_seq const* seq,
                                  unsigned start, unsigned length);

#define IMM_SUBSEQ(name, seq, start, length)                                                  \
    struct imm_subseq name = imm_subseq_init(&(name), seq, start, length)

struct imm_seq const* imm_subseq_cast(struct imm_subseq const* subseq);

void imm_subseq_set(struct imm_subseq* subseq, unsigned start, unsigned length);

#endif
