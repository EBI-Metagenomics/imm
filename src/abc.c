#include "abc.h"
#include "common/common.h"
#include "imm/bug.h"
#include "imm/error.h"
#include "imm/sym.h"

struct imm_abc const imm_abc_empty = {
    .nsymbols = 0,
    .symbols = NULL,
    .sym = {.idx[0 ...(IMM_SYM_SIZE) - 1] = IMM_SYM_NULL_IDX},
    .any_symbol_id = IMM_SYM_ID('X'),
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
    if (!imm_sym_valid_char(any_symbol))
        return xerror(IMM_ILLEGALARG, "any_symbol outside range");

    if (len == 0)
        return xerror(IMM_ILLEGALARG, "alphabet cannot be empty");

    if (len > IMM_SYM_SIZE)
        return xerror(IMM_ILLEGALARG, "symbols length is too large");

    abc->nsymbols = len;
    abc->symbols = symbols;
    imm_sym_init(&abc->sym);
    abc->any_symbol_id = imm_sym_id(any_symbol);

    for (unsigned i = 0; i < abc->nsymbols; ++i)
    {
        if (symbols[i] == any_symbol)
            return xerror(IMM_ILLEGALARG,
                          "any_symbol cannot be in the alphabet");

        if (!imm_sym_valid_char(symbols[i]))
            return xerror(IMM_ILLEGALARG, "symbol outside range");

        unsigned id = imm_sym_id(symbols[i]);
        if (imm_sym_idx(&abc->sym, id) != IMM_SYM_NULL_IDX)
            return xerror(IMM_ILLEGALARG,
                          "alphabet cannot have duplicated symbols");

        imm_sym_set_idx(&abc->sym, id, i);
    }
    imm_sym_set_idx(&abc->sym, abc->any_symbol_id, abc->nsymbols);
    abc->vtable = vtable;
    return IMM_SUCCESS;
}
