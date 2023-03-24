#ifndef IMM_SEQ_H
#define IMM_SEQ_H

#include "imm/abc.h"
#include "imm/export.h"
#include "imm/str.h"

struct imm_seq
{
    unsigned size;
    char const *str;
    struct imm_abc const *abc;
};

extern struct imm_seq const imm_seq_empty;

#define IMM_SEQ_UNSAFE(size, str, abc)                                         \
    (struct imm_seq) { size, str, abc }

static inline struct imm_abc const *imm_seq_abc(struct imm_seq const *seq)
{
    return seq->abc;
}

IMM_API struct imm_seq imm_seq(struct imm_str str, struct imm_abc const *abc);

static inline unsigned imm_seq_size(struct imm_seq const *seq)
{
    return seq->size;
}
static inline char const *imm_seq_str(struct imm_seq const *seq)
{
    return seq->str;
}

static inline unsigned imm_seq_symbol_idx(struct imm_seq const *seq, unsigned i)
{
    return imm_abc_symbol_idx(seq->abc, seq->str[i]);
}

#endif
