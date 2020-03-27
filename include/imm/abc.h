#ifndef IMM_ABC_H
#define IMM_ABC_H

#include "imm/export.h"
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>

#define IMM_ABC_INVALID_IDX UINT_MAX

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
    unsigned    symbol_idx[IMM_SYMBOL_IDX_SIZE];
    char        any_symbol;
};

static inline size_t __imm_abc_index(char const c) { return (size_t)(c - IMM_FIRST_CHAR); }

IMM_EXPORT struct imm_abc const* imm_abc_create(char const* symbols, char any_symbol);
IMM_EXPORT struct imm_abc const* imm_abc_clone(struct imm_abc const* abc);
IMM_EXPORT void                  imm_abc_destroy(struct imm_abc const* abc);
static inline unsigned    imm_abc_length(struct imm_abc const* abc) { return abc->length; }
static inline char const* imm_abc_symbols(struct imm_abc const* abc) { return abc->symbols; }
static inline bool        imm_abc_has_symbol(struct imm_abc const* abc, char symbol_id)
{
    if (symbol_id < IMM_FIRST_CHAR || symbol_id > IMM_LAST_CHAR)
        return false;

    return abc->symbol_idx[__imm_abc_index(symbol_id)] != IMM_ABC_INVALID_IDX;
}
static inline unsigned imm_abc_symbol_idx(struct imm_abc const* abc, char symbol_id)
{
    return abc->symbol_idx[__imm_abc_index(symbol_id)];
}
static inline char imm_abc_symbol_id(struct imm_abc const* abc, unsigned symbol_idx)
{
    return abc->symbols[symbol_idx];
}
static inline char imm_abc_any_symbol(struct imm_abc const* abc) { return abc->any_symbol; }
IMM_EXPORT enum imm_symbol_type imm_abc_symbol_type(struct imm_abc const* abc, char symbol_id);

#endif
