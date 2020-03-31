#ifndef IMM_SEQ_CODE_H
#define IMM_SEQ_CODE_H

#include "imm/export.h"

struct imm_abc;
struct imm_seq;
struct eseq;

struct seq_code
{
    unsigned              min_seq;
    unsigned              max_seq;
    unsigned*             offset;
    unsigned*             stride;
    unsigned              size;
    struct imm_abc const* abc;
};

/* TODO: remove IMM_EXPORT */
IMM_EXPORT struct seq_code const* seq_code_create(struct imm_abc const* abc, unsigned min_seq,
                                                  unsigned max_seq);
IMM_EXPORT unsigned               seq_code_encode(struct seq_code const* seq_code,
                                                  struct imm_seq const*  seq);
static inline unsigned seq_code_offset(struct seq_code const* seq_code, unsigned min_seq)
{
    return seq_code->offset[min_seq - seq_code->min_seq];
}
IMM_EXPORT struct eseq const* seq_code_create_eseq(struct seq_code const* seq_code,
                                                   struct imm_seq const*  seq);
IMM_EXPORT struct eseq*       seq_code_create_eseq2(struct seq_code const* seq_code);
IMM_EXPORT unsigned           seq_code_size(struct seq_code const* seq_code, unsigned min_seq);
IMM_EXPORT unsigned           seq_code_min_seq(struct seq_code const* seq_code);
IMM_EXPORT unsigned           seq_code_max_seq(struct seq_code const* seq_code);
IMM_EXPORT struct imm_abc const* seq_code_abc(struct seq_code const* seq_code);
IMM_EXPORT void                  seq_code_destroy(struct seq_code const* seq_code);

#endif
