#ifndef IMM_ABC_H
#define IMM_ABC_H

#include "imm/api.h"

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

struct imm_abc;

IMM_API struct imm_abc const* imm_abc_create(char const* symbols, char any_symbol);
IMM_API struct imm_abc const* imm_abc_clone(struct imm_abc const* abc);
IMM_API void                  imm_abc_destroy(struct imm_abc const* abc);
IMM_API int                   imm_abc_length(struct imm_abc const* abc);
IMM_API char const*           imm_abc_symbols(struct imm_abc const* abc);
IMM_API int                   imm_abc_has_symbol(struct imm_abc const* abc, char symbol_id);
IMM_API int                   imm_abc_symbol_idx(struct imm_abc const* abc, char symbol_id);
IMM_API char                  imm_abc_symbol_id(struct imm_abc const* abc, int symbol_idx);
IMM_API char                  imm_abc_any_symbol(struct imm_abc const* abc);
IMM_API enum imm_symbol_type  imm_abc_symbol_type(struct imm_abc const* abc, char symbol_id);

#endif
