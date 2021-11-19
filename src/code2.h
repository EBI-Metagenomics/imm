#ifndef CODE_H
#define CODE_H

#include "imm/code.h"

struct imm_abc;
struct imm_code;
struct imm_seq;

unsigned code2_encode(struct imm_code const *code, struct imm_seq const *seq,
                      unsigned min_seq);
void code2_init(struct imm_code *code, struct imm_abc const *abc);

static inline unsigned code2_size(struct imm_code const *code, unsigned min_seq,
                                  unsigned max_seq)
{
    return (unsigned)(code->offset[max_seq + 1] - code->offset[min_seq]);
}

#endif
