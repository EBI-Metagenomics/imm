#ifndef CODE_H
#define CODE_H

#include "imm/code.h"
#include <assert.h>

struct imm_abc;
struct imm_code;
struct imm_seq;

unsigned imm_code_encode(struct imm_code const *code,
                         struct imm_seq const *seq);

static inline unsigned code_translate(struct imm_code const *code,
                                      unsigned value, unsigned min_seq)
{
    assert(value >= code->offset[min_seq]);
    return value - code->offset[min_seq];
}

static inline unsigned code_size(struct imm_code const *code, unsigned min_seq,
                                 unsigned max_seq)
{
    return (unsigned)(code->offset[max_seq + 1] - code->offset[min_seq]);
}

#define CODE_EMPTY                                                             \
    {                                                                          \
        .offset = {0}, .stride = {0}, .abc = NULL                              \
    }

#endif
