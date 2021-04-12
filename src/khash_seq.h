#ifndef IMM_KHASH_SEQ_H
#define IMM_KHASH_SEQ_H

#include "imm/imm.h"
#include "khash.h"
#include "util.h"

static inline int     seq_hash_equal(struct imm_seq const* a, struct imm_seq const* b);
static inline khint_t seq_hash_func(struct imm_seq const* seq);

#define KHASH_MAP_INIT_SEQ(name, khval_t)                                                                              \
    KHASH_INIT(name, struct imm_seq const*, khval_t, 1, seq_hash_func, seq_hash_equal)

static inline int seq_hash_equal(struct imm_seq const* a, struct imm_seq const* b)
{
    if (imm_seq_length(a) != imm_seq_length(b))
        return 0;
    size_t n = MIN(imm_seq_length(a), imm_seq_length(b));
    return strncmp(imm_seq_string(a), imm_seq_string(b), n) == 0;
}

static inline khint_t seq_hash_func(struct imm_seq const* seq)
{
    const char* s = imm_seq_string(seq);
    khint_t     h = (khint_t)'\0';
    for (unsigned i = 0; i < imm_seq_length(seq); ++i) {
        h = (h << 5) - h + (khint_t)*s;
        ++s;
    }
    return h;
}

#endif
