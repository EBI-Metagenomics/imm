#ifndef IMM_SEQ_H
#define IMM_SEQ_H

#include "imm/export.h"
#include "imm/str.h"

struct imm_abc;

/**
 * Sequence of characters. No need for null-terminated string.
 */
struct imm_seq
{
    unsigned len;
    char const *str;
    struct imm_abc const *abc;
};

#define IMM_SEQ_UNSAFE(len, str, abc)                                          \
    (struct imm_seq) { len, str, abc }

static inline struct imm_abc const *imm_seq_abc(struct imm_seq const *seq)
{
    return seq->abc;
}

IMM_API struct imm_seq const *imm_seq_new(struct imm_str str,
                                          struct imm_abc const *abc);

IMM_API void imm_seq_del(struct imm_seq const *seq);

static inline unsigned imm_seq_len(struct imm_seq const *seq)
{
    return seq->len;
}
static inline char const *imm_seq_str(struct imm_seq const *seq)
{
    return seq->str;
}

#endif
