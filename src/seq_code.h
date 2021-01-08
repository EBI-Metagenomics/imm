#ifndef SEQ_CODE_H
#define SEQ_CODE_H

#include "imm/export.h"
#include <inttypes.h>
#include <stdio.h>

struct eseq;
struct imm_abc;
struct imm_io;
struct imm_seq;

struct seq_code
{
    uint8_t               min_seq;
    uint8_t               max_seq;
    uint16_t*             offset;
    uint16_t*             stride;
    uint16_t              size;
    struct imm_abc const* abc;
};

static inline struct imm_abc const* seq_code_abc(struct seq_code const* seq_code);
struct seq_code const* seq_code_create(struct imm_abc const* abc, uint8_t min_seq, uint8_t max_seq);
struct eseq*           seq_code_create_eseq(struct seq_code const* seq_code);
void                   seq_code_destroy(struct seq_code const* seq_code);
uint16_t               seq_code_encode(struct seq_code const* seq_code, struct imm_seq const* seq);
static inline uint8_t  seq_code_max_seq(struct seq_code const* seq_code);
static inline uint8_t  seq_code_min_seq(struct seq_code const* seq_code);
static inline uint16_t seq_code_offset(struct seq_code const* seq_code, uint8_t min_seq);
struct seq_code const* seq_code_read(FILE* stream, struct imm_abc const* abc);
static inline uint16_t seq_code_size(struct seq_code const* seq_code, uint8_t min_seq);
int                    seq_code_write(struct seq_code const* seq_code, FILE* stream);

static inline struct imm_abc const* seq_code_abc(struct seq_code const* seq_code)
{
    return seq_code->abc;
}

static inline uint8_t seq_code_max_seq(struct seq_code const* seq_code)
{
    return seq_code->max_seq;
}

static inline uint8_t seq_code_min_seq(struct seq_code const* seq_code)
{
    return seq_code->min_seq;
}

static inline uint16_t seq_code_offset(struct seq_code const* seq_code, uint8_t min_seq)
{
    return seq_code->offset[min_seq - seq_code->min_seq];
}

static inline uint16_t seq_code_size(struct seq_code const* seq_code, uint8_t min_seq)
{
    return seq_code->size - seq_code->offset[min_seq - seq_code->min_seq];
}

#endif
