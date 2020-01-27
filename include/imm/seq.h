#ifndef IMM_SEQ_H
#define IMM_SEQ_H

#include "imm/api.h"
#include <stdbool.h>

/**
 * Sequence of characters (not null-terminated string).
 */
struct imm_seq
{
    char const* string;
    unsigned    length;
};

#define IMM_SEQ(string)                                                                       \
    (struct imm_seq) { string, (sizeof(string) - 1) }

struct imm_seq imm_seq_duplicate(struct imm_seq seq);
void           imm_seq_destroy(struct imm_seq seq);

#endif
