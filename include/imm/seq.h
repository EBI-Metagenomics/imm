#ifndef IMM_SEQ_H
#define IMM_SEQ_H

#include "imm/export.h"

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

IMM_EXPORT struct imm_seq const*    imm_seq_create(char const* seq, struct imm_abc const* abc);
static inline struct imm_abc const* imm_seq_get_abc(struct imm_seq const* seq)
{
    return seq->abc;
}
static inline unsigned    imm_seq_length(struct imm_seq const* seq) { return seq->length; }
static inline char const* imm_seq_string(struct imm_seq const* seq) { return seq->string; }
IMM_EXPORT struct imm_seq const* imm_seq_clone(struct imm_seq const* seq);
IMM_EXPORT void                  imm_seq_destroy(struct imm_seq const* seq);

#endif
