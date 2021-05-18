#ifndef DP_CODE_H
#define DP_CODE_H

#include <stdint.h>

struct imm_abc;
struct imm_seq;

struct code
{
    struct
    {
        unsigned min;
        unsigned max;
    } seqlen;
    uint16_t *offset;
    uint16_t *stride;
    unsigned size;
    struct imm_abc const *abc;
};

void code_del(struct code const *code);

unsigned code_encode(struct code const *code, struct imm_seq const *seq);

void code_init(struct code *code, struct imm_abc const *abc);

static inline unsigned code_offset(struct code const *code, unsigned min_seq)
{
    return (unsigned)(code->offset[min_seq - code->seqlen.min]);
}

void code_reset(struct code *code, unsigned min_seq, unsigned max_seq);

static inline unsigned code_size(struct code const *code, unsigned min_seq)
{
    return (unsigned)(code->size - code->offset[min_seq - code->seqlen.min]);
}

static inline unsigned code_offset_size(struct code const *code)
{
    return (unsigned)(code->seqlen.max - code->seqlen.min + 1);
}

static inline unsigned code_stride_size(struct code const *code)
{
    return code->seqlen.max;
}

#endif
