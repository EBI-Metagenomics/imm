#ifndef IMM_SEQ_H
#define IMM_SEQ_H

#include "imm/api.h"

struct imm_abc;

/**
 *
 * Sequence of characters. No need for null-terminated string.
 */
struct imm_seq
{
    struct imm_abc const* abc;
    char const*           string;
    unsigned              length;
};

IMM_API struct imm_seq const* imm_seq_create(char const* seq, struct imm_abc const* abc);
IMM_API static inline struct imm_abc const* imm_seq_get_abc(struct imm_seq const* seq)
{
    return seq->abc;
}
IMM_API static inline unsigned imm_seq_length(struct imm_seq const* seq)
{
    return seq->length;
}
IMM_API static inline char const* imm_seq_string(struct imm_seq const* seq)
{
    return seq->string;
}
IMM_API struct imm_seq const* imm_seq_clone(struct imm_seq const* seq);
IMM_API void                  imm_seq_destroy(struct imm_seq const* seq);

#endif
