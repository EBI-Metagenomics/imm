#ifndef IMM_SEQ_H
#define IMM_SEQ_H

#include "imm/export.h"
#include <stdint.h>

struct imm_abc;

/**
 * Sequence of characters. No need for null-terminated string.
 */
struct imm_seq
{
    struct imm_abc const* abc;
    char const*           string;
    uint32_t              length;
};

#define IMM_SEQ_MAX_LEN UINT32_MAX

static inline struct imm_seq        IMM_SEQ(struct imm_abc const* abc, char const* string,
                                            uint32_t length);
IMM_API struct imm_seq const*       imm_seq_clone(struct imm_seq const* seq);
IMM_API struct imm_seq const*       imm_seq_create(char const* string, struct imm_abc const* abc);
IMM_API void                        imm_seq_destroy(struct imm_seq const* seq);
static inline struct imm_abc const* imm_seq_get_abc(struct imm_seq const* seq) { return seq->abc; }
static inline uint32_t    imm_seq_length(struct imm_seq const* seq) { return seq->length; }
static inline char const* imm_seq_string(struct imm_seq const* seq) { return seq->string; }

static inline struct imm_seq IMM_SEQ(struct imm_abc const* abc, char const* string, uint32_t length)
{
    return (struct imm_seq){abc, string, length};
}

#endif
