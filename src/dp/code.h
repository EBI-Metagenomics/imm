#ifndef DP_CODE_H
#define DP_CODE_H

#include "imm/dp/code.h"

struct imm_seq;

void code_del(struct imm_dp_code const *code);

unsigned code_encode(struct imm_dp_code const *code, struct imm_seq const *seq);

void code_init(struct imm_dp_code *code, struct imm_abc const *abc);

static inline unsigned code_offset(struct imm_dp_code const *code,
                                   unsigned min_seq)
{
    return (unsigned)(code->offset[min_seq - code->seqlen.min]);
}

enum imm_rc code_reset(struct imm_dp_code *code, unsigned min_seq,
                       unsigned max_seq);

static inline unsigned code_size(struct imm_dp_code const *code,
                                 unsigned min_seq)
{
    return (unsigned)(code->size - code->offset[min_seq - code->seqlen.min]);
}

static inline unsigned code_offset_size(struct imm_dp_code const *code)
{
    return (unsigned)(code->seqlen.max - code->seqlen.min + 1);
}

static inline unsigned code_stride_size(struct imm_dp_code const *code)
{
    return code->seqlen.max;
}

#endif
