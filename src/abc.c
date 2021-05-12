#include "abc.h"
#include "imm/sym.h"
#include "support.h"

struct imm_abc const imm_abc_empty = {
    .size = 0,
    .symbols = {'\0'},
    .sym = {.idx[0 ...(IMM_SYM_SIZE) - 1] = IMM_SYM_NULL_IDX},
    .any_symbol_id = IMM_SYM_ID('X'),
    .vtable = {IMM_ABC, NULL},
};

int imm_abc_init(struct imm_abc *abc, struct imm_str symbols, char any_symbol)
{
    struct imm_abc_vtable vtable = {IMM_ABC, NULL};
    return abc_init(abc, symbols.size, symbols.data, any_symbol, vtable);
}

int abc_init(struct imm_abc *abc, unsigned len, char const *symbols,
             char any_symbol, struct imm_abc_vtable vtable)
{
    if (!imm_sym_valid_char(any_symbol))
        return error(IMM_ILLEGALARG, "any_symbol outside range");

    if (len == 0)
        return error(IMM_ILLEGALARG, "alphabet cannot be empty");

    if (len > IMM_MAX_SYMBOLS_SIZE || len > IMM_SYM_SIZE)
        return error(IMM_ILLEGALARG, "symbols length is too large");

    abc->size = len;
    xmemcpy(abc->symbols, symbols, sizeof(*abc->symbols) * len);
    abc->symbols[abc->size] = '\0';
    imm_sym_init(&abc->sym);
    abc->any_symbol_id = imm_sym_id(any_symbol);

    for (unsigned i = 0; i < abc->size; ++i)
    {
        if (symbols[i] == any_symbol)
            return error(IMM_ILLEGALARG,
                         "any_symbol cannot be in the alphabet");

        if (!imm_sym_valid_char(symbols[i]))
            return error(IMM_ILLEGALARG, "symbol outside range");

        unsigned id = imm_sym_id(symbols[i]);
        if (imm_sym_idx(&abc->sym, id) != IMM_SYM_NULL_IDX)
            return error(IMM_ILLEGALARG,
                         "alphabet cannot have duplicated symbols");

        imm_sym_set_idx(&abc->sym, id, i);
    }
    imm_sym_set_idx(&abc->sym, abc->any_symbol_id, abc->size);
    abc->vtable = vtable;
    return IMM_SUCCESS;
}
