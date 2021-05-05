#include "abc.h"
#include "common/common.h"
#include "imm/error.h"
#include "imm/sym.h"

struct imm_abc const imm_abc_empty = {
    .nsymbols = 0,
    .symbols = NULL,
    .symbol_idx[0 ...(__IMM_SYM_IDX_SIZE) - 1] = IMM_SYM_NULL_IDX,
    .any_symbol = 'X',
    .vtable = {IMM_ABC, NULL},
};

int imm_abc_init(struct imm_abc *abc, struct imm_str symbols, char any_symbol)
{
    struct imm_abc_vtable vtable = {IMM_ABC, NULL};
    return abc_init(abc, symbols.len, symbols.data, any_symbol, vtable);
}

int abc_init(struct imm_abc *abc, unsigned len, char const *symbols,
             char any_symbol, struct imm_abc_vtable vtable)
{
    if (!__imm_sym_valid(any_symbol))
        return xerror(IMM_ILLEGALARG, "any_symbol outside range");

    if (len == 0)
        return xerror(IMM_ILLEGALARG, "alphabet cannot be empty");

    if (len > __IMM_SYM_IDX_SIZE)
        return xerror(IMM_ILLEGALARG, "symbols length is too large");

    abc->any_symbol = any_symbol;

    for (unsigned i = 0; i < ARRAY_SIZE(abc->symbol_idx); ++i)
        abc->symbol_idx[i] = IMM_SYM_NULL_IDX;

    abc->nsymbols = len;
    for (unsigned i = 0; i < abc->nsymbols; ++i)
    {
        if (symbols[i] == any_symbol)
            return xerror(IMM_ILLEGALARG,
                          "any_symbol cannot be in the alphabet");

        if (!__imm_sym_valid(symbols[i]))
            return xerror(IMM_ILLEGALARG, "symbol outside range");

        unsigned j = __imm_sym_index(symbols[i]);
        if (abc->symbol_idx[j] != IMM_SYM_NULL_IDX)
            return xerror(IMM_ILLEGALARG,
                          "alphabet cannot have duplicated symbols");

        abc->symbol_idx[j] = (__imm_sym_idx_t)i;
    }
    abc->symbols = symbols;
    abc->vtable = vtable;
    return IMM_SUCCESS;
}
