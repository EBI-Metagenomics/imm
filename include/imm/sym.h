#ifndef IMM_SYM_H
#define IMM_SYM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t __imm_sym_idx_t;

enum imm_sym_type
{
    IMM_SYM_NULL = 0,
    IMM_SYM_NORMAL = 1,
    IMM_SYM_ANY = 2,
};

#define __IMM_SYM_FIRST '!'
#define __IMM_SYM_LAST '~'

#define IMM_SYM_NULL_IDX UINT8_MAX

#define __IMM_SYM_IDX_SIZE ((size_t)((__IMM_SYM_LAST - __IMM_SYM_FIRST) + 1))

#define __IMM_SYM_INDEX(c) ((unsigned)(c - __IMM_SYM_FIRST))

static inline unsigned __imm_sym_index(char c) { return __IMM_SYM_INDEX(c); }

static inline bool __imm_sym_valid(char symbol)
{
    return symbol >= __IMM_SYM_FIRST && symbol <= __IMM_SYM_LAST;
}

#endif
