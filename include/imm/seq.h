#ifndef IMM_SEQ_H
#define IMM_SEQ_H

#include "imm/export.h"
#include <stdint.h>
#include <stdlib.h>

struct imm_abc;

/**
 * Sequence of characters. No need for null-terminated string.
 */
struct imm_seq
{
    uint32_t len;
    char const *str;
    struct imm_abc const *abc;
};

#define IMM_SEQ(len, str, abc)                                                 \
    (struct imm_seq) { abc, len, str }

static inline struct imm_abc const *imm_seq_abc(struct imm_seq const *seq)
{
    return seq->abc;
}

IMM_API struct imm_seq const *imm_seq_new(uint32_t len, char const *str,
                                          struct imm_abc const *abc);

static inline void imm_seq_del(struct imm_seq const *seq) { free((void *)seq); }

static inline uint32_t imm_seq_len(struct imm_seq const *seq)
{
    return seq->len;
}
static inline char const *imm_seq_str(struct imm_seq const *seq)
{
    return seq->str;
}

#endif
