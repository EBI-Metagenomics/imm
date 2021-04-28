#include "abc.h"
#include "common/common.h"
#include "imm/error.h"
#include "imm/sym.h"

static struct imm_abc_vtable const __vtable = {abc_typeid, abc_del};

void imm_abc_del(struct imm_abc const *abc) { abc->vtable.del(abc); }

struct imm_abc const *imm_abc_new(unsigned len, char const *symbols,
                                  char any_symbol)
{
    return abc_new(len, symbols, any_symbol, NULL);
}

struct imm_abc *abc_new(unsigned len, char const *symbols, char any_symbol,
                        void *derived)
{
    if (!__imm_sym_valid(any_symbol))
    {
        xerror(IMM_ILLEGALARG, "any_symbol outside range");
        return NULL;
    }
    if (len == 0)
    {
        xerror(IMM_ILLEGALARG, "alphabet cannot be empty");
        return NULL;
    }
    if (len > IMM_SYM_IDX_SIZE)
    {
        xerror(IMM_ILLEGALARG, "symbols length is too large");
        return NULL;
    }

    struct imm_abc *abc = xmalloc(sizeof(*abc));
    abc->any_symbol = any_symbol;

    for (unsigned i = 0; i < ARRAY_SIZE(abc->symbol_idx); ++i)
        abc->symbol_idx[i] = IMM_SYM_NULL_IDX;

    abc->nsymbols = len;
    for (unsigned i = 0; i < abc->nsymbols; ++i)
    {
        if (symbols[i] == any_symbol)
        {
            xerror(IMM_ILLEGALARG, "any_symbol cannot be in the alphabet");
            goto cleanup;
        }

        if (!__imm_sym_valid(symbols[i]))
        {
            xerror(IMM_ILLEGALARG, "symbol outside range");
            goto cleanup;
        }

        unsigned j = __imm_sym_index(symbols[i]);
        if (abc->symbol_idx[j] != IMM_SYM_NULL_IDX)
        {
            xerror(IMM_ILLEGALARG, "alphabet cannot have duplicated symbols");
            goto cleanup;
        }
        abc->symbol_idx[j] = (imm_sym_idx_t)i;
    }
    abc->symbols = xstrdup(symbols);
    abc->vtable = __vtable;
    abc->derived = derived;

    return abc;

cleanup:
    free(abc);
    return NULL;
}

void abc_del(struct imm_abc const *abc)
{
    free((void *)abc->symbols);
    free((void *)abc);
}
