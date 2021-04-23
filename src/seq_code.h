#ifndef SEQ_CODE_H
#define SEQ_CODE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct eseq;
struct imm_abc;
struct imm_seq;

struct seq_code
{
    uint8_t min_seq;
    uint8_t max_seq;
    uint16_t *offset;
    uint16_t *stride;
    uint16_t size;
    struct imm_abc const *abc;
};

static inline void seq_code_del(struct seq_code const *seq_code)
{
    free(seq_code->offset);
    free(seq_code->stride);
    free((void *)seq_code);
}

uint_fast16_t seq_code_encode(struct seq_code const *seq_code,
                              struct imm_seq const *seq);

struct eseq *seq_code_new_eseq(struct seq_code const *seq_code);

struct seq_code *seq_code_new(struct imm_abc const *abc, unsigned min_seq,
                              unsigned max_seq);

static inline uint_fast16_t seq_code_offset(struct seq_code const *seq_code,
                                            unsigned min_seq)
{
    return (uint_fast16_t)(seq_code->offset[min_seq - seq_code->min_seq]);
}

struct seq_code *seq_code_reset(struct seq_code *seq_code,
                                struct imm_abc const *abc, unsigned min_seq,
                                unsigned max_seq);

/* struct seq_code const *seq_code_read(FILE *stream, struct imm_abc const
 * *abc); */

static inline uint_fast16_t seq_code_size(struct seq_code const *seq_code,
                                          unsigned min_seq)
{
    return (uint_fast16_t)(seq_code->size -
                           seq_code->offset[min_seq - seq_code->min_seq]);
}

/* int seq_code_write(struct seq_code const *seq_code, FILE *stream); */

#endif
