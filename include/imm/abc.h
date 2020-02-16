#ifndef IMM_ABC_H
#define IMM_ABC_H

#include "imm/api.h"
#include <stdbool.h>
#include <stddef.h>

/** @file abc.h
 * Alphabet module.
 *
 * An alphabet is represented by an (immutable) object of type @ref imm_abc.
 */

enum imm_symbol_type
{
    IMM_SYMBOL_UNKNOWN = 0,
    IMM_SYMBOL_NORMAL = 1,
    IMM_SYMBOL_ANY = 2,
};

#define IMM_FIRST_CHAR '!'
#define IMM_LAST_CHAR '~'

#define IMM_SYMBOL_IDX_SIZE ((size_t)((IMM_LAST_CHAR - IMM_FIRST_CHAR) + 1))

/**
 * Immutable alphabet.
 *
 * It represents a finite set of symbols and a special any-symbol
 * symbol.
 */
struct imm_abc
{
    char const* symbols;
    unsigned    length;
    int         symbol_idx[IMM_SYMBOL_IDX_SIZE];
    char        any_symbol;
};

static inline size_t __imm_abc_index(char const c) { return (size_t)(c - IMM_FIRST_CHAR); }

IMM_API struct imm_abc const*  imm_abc_create(char const* symbols, char any_symbol);
IMM_API struct imm_abc const*  imm_abc_clone(struct imm_abc const* abc);
IMM_API void                   imm_abc_destroy(struct imm_abc const* abc);
IMM_API static inline unsigned imm_abc_length(struct imm_abc const* abc)
{
    return abc->length;
}
IMM_API static inline char const* imm_abc_symbols(struct imm_abc const* abc)
{
    return abc->symbols;
}
IMM_API static inline bool imm_abc_has_symbol(struct imm_abc const* abc, char symbol_id)
{

    if (symbol_id < IMM_FIRST_CHAR || symbol_id > IMM_LAST_CHAR)
        return false;

    return abc->symbol_idx[__imm_abc_index(symbol_id)] != -1;
}
IMM_API static inline int imm_abc_symbol_idx(struct imm_abc const* abc, char symbol_id)
{

    if (symbol_id < IMM_FIRST_CHAR || symbol_id > IMM_LAST_CHAR)
        return -1;
    return abc->symbol_idx[__imm_abc_index(symbol_id)];
}
IMM_API static inline char imm_abc_symbol_id(struct imm_abc const* abc, unsigned symbol_idx)
{
    return abc->symbols[symbol_idx];
}
IMM_API static inline char imm_abc_any_symbol(struct imm_abc const* abc)
{
    return abc->any_symbol;
}
IMM_API enum imm_symbol_type imm_abc_symbol_type(struct imm_abc const* abc, char symbol_id);

#endif
