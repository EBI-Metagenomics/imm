#ifndef IMM_SEQ_H
#define IMM_SEQ_H

#include "imm/abc.h"
#include "imm/bug.h"
#include "imm/error.h"
#include "imm/export.h"
#include "imm/str.h"

struct imm_abc;

struct imm_seq
{
    unsigned len;
    char const *str;
    struct imm_abc const *abc;
};

extern struct imm_seq const imm_seq_empty;

#define IMM_SEQ_UNSAFE(len, str, abc)                                          \
    (struct imm_seq) { len, str, abc }

static inline struct imm_abc const *imm_seq_abc(struct imm_seq const *seq)
{
    return seq->abc;
}

static inline struct imm_seq imm_seq_init(struct imm_str str,
                                          struct imm_abc const *abc)
{
    for (unsigned i = 0; i < str.len; ++i)
    {
        if (!imm_abc_has_symbol(abc, str.data[i]) &&
            str.data[i] != imm_abc_any_symbol(abc))
        {
            imm_die(IMM_ILLEGALARG, "invalid sequence");
        }
    }

    return (struct imm_seq){str.len, str.data, abc};
}

static inline unsigned imm_seq_len(struct imm_seq const *seq)
{
    return seq->len;
}
static inline char const *imm_seq_str(struct imm_seq const *seq)
{
    return seq->str;
}

#endif
