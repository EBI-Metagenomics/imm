#ifndef IMM_SYM_H
#define IMM_SYM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t imm_sym_idx_t;

enum imm_sym_type
{
    IMM_SYM_NULL = 0,
    IMM_SYM_NORMAL = 1,
    IMM_SYM_ANY = 2,
};

#define IMM_SYM_FIRST '!'
#define IMM_SYM_LAST '~'

#define IMM_SYM_NULL_IDX UINT8_MAX

#define IMM_SYM_IDX_SIZE ((size_t)((IMM_SYM_LAST - IMM_SYM_FIRST) + 1))

static inline imm_sym_idx_t __imm_sym_index(char const c)
{
    return (imm_sym_idx_t)(c - IMM_SYM_FIRST);
}

static inline bool __imm_sym_valid(char const symbol)
{
    return symbol >= IMM_SYM_FIRST && symbol <= IMM_SYM_LAST;
}

#endif
