#ifndef CODE_H
#define CODE_H

#include "imm/code.h"

struct imm_abc;
struct imm_code;
struct imm_seq;

unsigned code2_encode(struct imm_code const *code, struct imm_seq const *seq);
void code2_init(struct imm_code *code, struct imm_abc const *abc);
void code2_reset(struct imm_code *code, unsigned min_seq, unsigned max_seq);

static inline unsigned code2_size(struct imm_code const *code, unsigned min_seq,
                                  unsigned max_seq)
{
    unsigned i = min_seq - code->seqlen.min;
    unsigned j = max_seq - code->seqlen.min;
    return (unsigned)(code->offset[j + 1] - code->offset[i]);
}

static inline unsigned code2_size_total(struct imm_code const *code)
{
    return code2_size(code, code->seqlen.min, code->seqlen.max);
}

#endif
