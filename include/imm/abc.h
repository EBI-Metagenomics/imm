#ifndef IMM_ABC_H
#define IMM_ABC_H

#include "imm/abc_types.h"
#include "imm/bug.h"
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
    struct imm_sym sym;
    unsigned any_symbol_id;
    struct imm_abc_vtable vtable;
};

extern struct imm_abc const imm_abc_empty;

static inline char imm_abc_any_symbol(struct imm_abc const *abc)
{
    return imm_sym_char(abc->any_symbol_id);
}

IMM_API int imm_abc_init(struct imm_abc *abc, struct imm_str symbols,
                         char any_symbol_id);

IMM_API void imm_abc_del(struct imm_abc const *abc);

static inline bool imm_abc_has_symbol_id(struct imm_abc const *abc, unsigned id)
{
    if (imm_unlikely(!imm_sym_valid_id(id)))
        return false;

    return imm_sym_idx(&abc->sym, id) < abc->nsymbols;
}

static inline bool imm_abc_has_symbol(struct imm_abc const *abc, char symbol)
{
    return imm_abc_has_symbol_id(abc, imm_sym_id(symbol));
}

static inline unsigned imm_abc_len(struct imm_abc const *abc)
{
    return abc->nsymbols;
}

static inline char imm_abc_symbol(struct imm_abc const *abc, char symbol)
{
    unsigned id = imm_sym_id(symbol);
    IMM_BUG(!imm_abc_has_symbol_id(abc, id) && abc->any_symbol_id != id);
    return abc->symbols[imm_sym_idx(&abc->sym, id)];
}

static inline unsigned __imm_abc_symbol_idx(struct imm_abc const *abc,
                                            unsigned id)
{
    IMM_BUG(!imm_abc_has_symbol_id(abc, id) && id != abc->any_symbol_id);
    return imm_sym_idx(&abc->sym, id);
}

static inline unsigned imm_abc_symbol_idx(struct imm_abc const *abc,
                                          char symbol)
{
    return __imm_abc_symbol_idx(abc, imm_sym_id(symbol));
}

static inline bool imm_abc_any_symbol_idx(struct imm_abc const *abc)
{
    return abc->nsymbols;
}

static inline enum imm_sym_type __imm_abc_symbol_type(struct imm_abc const *abc,
                                                      unsigned id)
{
    if (id == abc->any_symbol_id)
        return IMM_SYM_ANY;

    if (imm_abc_has_symbol_id(abc, id))
        return IMM_SYM_NORMAL;

    return IMM_SYM_NULL;
}

static inline enum imm_sym_type imm_abc_symbol_type(struct imm_abc const *abc,
                                                    char symbol)
{
    return __imm_abc_symbol_type(abc, imm_sym_id(symbol));
}

static inline char const *imm_abc_symbols(struct imm_abc const *abc)
{
    return abc->symbols;
}

static inline enum imm_abc_typeid imm_abc_typeid(struct imm_abc const *abc)
{
    return abc->vtable.typeid;
}

#endif
