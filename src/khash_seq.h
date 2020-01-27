#ifndef IMM_KHASH_SEQ_H
#define IMM_KHASH_SEQ_H

#include "imm/seq.h"
#include "khash.h"
#include "min.h"

static kh_inline khint_t seq_hash_func(struct imm_seq const seq)
{
    const char* s = seq.string;
    khint_t     h = (khint_t)'\0';
    for (unsigned i = 0; i < seq.length; ++i) {
        h = (h << 5) - h + (khint_t)*s;
        ++s;
    }
    return h;
}

static kh_inline int seq_hash_equal(struct imm_seq const a, struct imm_seq const b)
{
    if (a.length != b.length)
        return 0;
    return strncmp(a.string, b.string, MIN(a.length, b.length)) == 0;
}

#define KHASH_MAP_INIT_SEQ(name, khval_t)                                                     \
    KHASH_INIT(name, struct imm_seq, khval_t, 1, seq_hash_func, seq_hash_equal)

#endif
