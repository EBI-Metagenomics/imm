#include "imm/abc.h"
#include "free.h"
#include "imm/report.h"
#include <string.h>

struct imm_abc const* imm_abc_create(char const* symbols, char const any_symbol)
{
    if (any_symbol < IMM_FIRST_CHAR || any_symbol > IMM_LAST_CHAR) {
        imm_error("any_symbol is outside the range [%c, %c] ", IMM_FIRST_CHAR, IMM_LAST_CHAR);
        return NULL;
    }
    struct imm_abc* abc = malloc(sizeof(struct imm_abc));
    abc->any_symbol = any_symbol;

    for (size_t i = 0; i < IMM_SYMBOL_IDX_SIZE; ++i)
        abc->symbol_idx[i] = IMM_ABC_INVALID_IDX;

    if (strlen(symbols) > IMM_SYMBOL_IDX_SIZE) {
        imm_error("alphabet size cannot be larger than %zu", IMM_SYMBOL_IDX_SIZE);
        imm_free(abc);
        return NULL;
    }

    abc->length = (unsigned)strlen(symbols);
    for (unsigned i = 0; i < abc->length; ++i) {
        if (symbols[i] == any_symbol) {
            imm_error("any_symbol cannot be in the alphabet");
            imm_free(abc);
            return NULL;
        }

        if (symbols[i] < IMM_FIRST_CHAR || symbols[i] > IMM_LAST_CHAR) {
            imm_error("alphabet symbol is outside the range [%c, %c] ", IMM_FIRST_CHAR,
                      IMM_LAST_CHAR);
            imm_free(abc);
            return NULL;
        }

        size_t j = __imm_abc_index(symbols[i]);
        if (abc->symbol_idx[j] != IMM_ABC_INVALID_IDX) {
            imm_error("alphabet cannot have duplicated symbols");
            imm_free(abc);
            return NULL;
        }
        abc->symbol_idx[j] = i;
    }
    abc->symbols = strdup(symbols);

    return abc;
}

struct imm_abc const* imm_abc_clone(struct imm_abc const* abc)
{
    struct imm_abc* nabc = malloc(sizeof(struct imm_abc));

    nabc->symbols = strdup(abc->symbols);
    nabc->length = abc->length;
    memcpy(nabc->symbol_idx, abc->symbol_idx, sizeof(unsigned) * IMM_SYMBOL_IDX_SIZE);
    nabc->any_symbol = abc->any_symbol;

    return nabc;
}

void imm_abc_destroy(struct imm_abc const* abc)
{
    imm_free(abc->symbols);
    imm_free(abc);
}

enum imm_symbol_type imm_abc_symbol_type(struct imm_abc const* abc, char symbol_id)
{
    if (symbol_id == abc->any_symbol)
        return IMM_SYMBOL_ANY;

    if (imm_abc_has_symbol(abc, symbol_id))
        return IMM_SYMBOL_NORMAL;

    return IMM_SYMBOL_UNKNOWN;
}
