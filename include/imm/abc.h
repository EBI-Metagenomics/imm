#ifndef IMM_ABC_H
#define IMM_ABC_H

#include "imm/abc_types.h"
#include "imm/compiler.h"
#include "imm/export.h"
#include "imm/str.h"
#include "imm/sym.h"

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
    __imm_sym_idx_t symbol_idx[__IMM_SYM_IDX_SIZE];
    char any_symbol;
    struct imm_abc_vtable vtable;
};

extern struct imm_abc const imm_abc_empty;

static inline char imm_abc_any_symbol(struct imm_abc const *abc)
{
    return abc->any_symbol;
}

IMM_API int imm_abc_init(struct imm_abc *abc, struct imm_str symbols,
                         char any_symbol);

IMM_API void imm_abc_del(struct imm_abc const *abc);

static inline bool imm_abc_has_symbol(struct imm_abc const *abc, char symbol)
{
    if (imm_unlikely(!__imm_sym_valid(symbol)))
        return false;

    return abc->symbol_idx[__imm_sym_index(symbol)] != IMM_SYM_NULL_IDX;
}

static inline unsigned imm_abc_len(struct imm_abc const *abc)
{
    return abc->nsymbols;
}

/* IMM_API struct imm_abc *imm_abc_read(FILE *stream); */

static inline char imm_abc_symbol(struct imm_abc const *abc, unsigned idx)
{
    return abc->symbols[idx];
}

static inline unsigned imm_abc_symbol_idx(struct imm_abc const *abc,
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

static inline enum imm_abc_typeid imm_abc_typeid(struct imm_abc const *abc)
{
    return abc->vtable.typeid;
}

/* IMM_API int imm_abc_write(struct imm_abc const *abc, FILE *stream); */

/* IMM_API int __imm_abc_write(struct imm_abc const *abc, FILE *stream); */

#endif
