#ifndef IMM_ABC_H
#define IMM_ABC_H

#include "imm/abc_types.h"
#include "imm/export.h"
#include "imm/log.h"
#include "imm/str.h"
#include "imm/support.h"
#include "imm/sym.h"
#include <stdio.h>

#define IMM_ABC_MAX_SIZE (31U)

/**
 * Alphabet.
 *
 * It represents a finite set of symbols and a special any-symbol
 * symbol. It should be used as an immutable object.
 */
struct imm_abc
{
    unsigned size;
    char symbols[IMM_ABC_MAX_SIZE + 1];
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

IMM_API int imm_abc_write(struct imm_abc const *abc, FILE *file);

IMM_API int imm_abc_read(struct imm_abc *abc, FILE *file);

static inline bool imm_abc_has_symbol_id(struct imm_abc const *abc, unsigned id)
{
    if (imm_unlikely(!imm_sym_valid_id(id)))
        return false;

    return imm_sym_idx(&abc->sym, id) < abc->size;
}

static inline bool imm_abc_has_symbol(struct imm_abc const *abc, char symbol)
{
    return imm_abc_has_symbol_id(abc, imm_sym_id(symbol));
}

static inline unsigned imm_abc_size(struct imm_abc const *abc)
{
    return abc->size;
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

static inline unsigned imm_abc_any_symbol_idx(struct imm_abc const *abc)
{
    return abc->size;
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
