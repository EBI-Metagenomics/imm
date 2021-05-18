#ifndef IMM_SYM_H
#define IMM_SYM_H

#include "imm/compiler.h"
#include "imm/export.h"
#include "imm/support.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t imm_sym_id_t;
typedef uint8_t imm_sym_idx_t;

enum imm_sym_type
{
    IMM_SYM_NULL = 0,
    IMM_SYM_NORMAL = 1,
    IMM_SYM_ANY = 2,
};

#define IMM_SYM_FIRST_CHAR '!'
#define IMM_SYM_LAST_CHAR '~'

#define IMM_SYM_SIZE ((size_t)((IMM_SYM_LAST_CHAR - IMM_SYM_FIRST_CHAR) + 1))

#define IMM_SYM_NULL_ID UINT8_MAX
#define IMM_SYM_NULL_IDX UINT8_MAX

#define IMM_SYM_ID(c) ((unsigned)(c - IMM_SYM_FIRST_CHAR))
#define IMM_SYM_CHAR(x) ((char)(x + IMM_SYM_FIRST_CHAR))

struct imm_sym
{
    imm_sym_idx_t idx[IMM_SYM_SIZE];
};

static inline void imm_sym_init(struct imm_sym *sym)
{
    for (unsigned i = 0; i < IMM_ARRAY_SIZE(sym->idx); ++i)
        sym->idx[i] = IMM_SYM_NULL_IDX;
}

static inline unsigned imm_sym_id(char c) { return IMM_SYM_ID(c); }

static inline char imm_sym_char(unsigned id) { return IMM_SYM_CHAR(id); }

static inline unsigned imm_sym_idx(struct imm_sym const *sym, unsigned id)
{
    return sym->idx[id];
}

static inline void imm_sym_set_idx(struct imm_sym *sym, unsigned id,
                                   unsigned idx)
{
    sym->idx[id] = (imm_sym_idx_t)idx;
}

static inline bool imm_sym_valid_char(char c)
{
    return imm_likely(c >= IMM_SYM_FIRST_CHAR && c <= IMM_SYM_LAST_CHAR);
}

static inline bool imm_sym_valid_id(unsigned id)
{
    return imm_sym_valid_char(imm_sym_char(id));
}

#endif
