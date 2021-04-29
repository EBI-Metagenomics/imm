#ifndef IMM_ABC_H
#define IMM_ABC_H

#include "imm/abc_types.h"
#include "imm/export.h"
#include "imm/sym.h"

struct imm_abc;

struct imm_abc_vtable
{
    void (*del)(struct imm_abc const *abc);
    imm_abc_tid_t typeid;
    void *derived;
};

/**
 * Alphabet.
 *
 * It represents a finite set of symbols and a special any-symbol
 * symbol. It should be used as an immutable object.
 */
struct imm_abc
{
    unsigned nsymbols;
    char const *symbols;
    imm_sym_idx_t symbol_idx[IMM_SYM_IDX_SIZE];
    char any_symbol;
    struct imm_abc_vtable vtable;
};

static inline char imm_abc_any_symbol(struct imm_abc const *abc)
{
    return abc->any_symbol;
}

IMM_API struct imm_abc const *imm_abc_new(unsigned len, char const *symbols,
                                          char any_symbol);

IMM_API void imm_abc_del(struct imm_abc const *abc);

static inline bool imm_abc_has_symbol(struct imm_abc const *abc, char symbol)
{
    if (!__imm_sym_valid(symbol))
        return false;

    return abc->symbol_idx[__imm_sym_index(symbol)] != IMM_SYM_NULL_IDX;
}

static inline unsigned imm_abc_len(struct imm_abc const *abc)
{
    return abc->nsymbols;
}

/* IMM_API struct imm_abc *imm_abc_read(FILE *stream); */

static inline char imm_abc_symbol(struct imm_abc const *abc, imm_sym_idx_t idx)
{
    return abc->symbols[idx];
}

static inline imm_abc_tid_t imm_abc_symbol_idx(struct imm_abc const *abc,
                                               char symbol)
{
    return abc->symbol_idx[__imm_sym_index(symbol)];
}

static inline enum imm_sym_type imm_abc_symbol_type(struct imm_abc const *abc,
                                                    char symbol)
{
    if (symbol == abc->any_symbol)
        return IMM_SYM_ANY;

    if (imm_abc_has_symbol(abc, symbol))
        return IMM_SYM_NORMAL;

    return IMM_SYM_NULL;
}

static inline char const *imm_abc_symbols(struct imm_abc const *abc)
{
    return abc->symbols;
}

static inline imm_abc_tid_t imm_abc_typeid(struct imm_abc const *abc)
{
    return abc->vtable.typeid;
}

/* IMM_API int imm_abc_write(struct imm_abc const *abc, FILE *stream); */

/* IMM_API int __imm_abc_write(struct imm_abc const *abc, FILE *stream); */

#endif
