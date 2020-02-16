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
        abc->symbol_idx[i] = -1;

    if (strlen(symbols) > IMM_SYMBOL_IDX_SIZE) {
        imm_error("alphabet size cannot be larger than %zu", IMM_SYMBOL_IDX_SIZE);
        free_c(abc);
        return NULL;
    }

    abc->length = (unsigned)strlen(symbols);
    for (unsigned i = 0; i < abc->length; ++i) {
        if (symbols[i] == any_symbol) {
            imm_error("any_symbol cannot be in the alphabet");
            free_c(abc);
            return NULL;
        }

        if (symbols[i] < IMM_FIRST_CHAR || symbols[i] > IMM_LAST_CHAR) {
            imm_error("alphabet symbol is outside the range [%c, %c] ", IMM_FIRST_CHAR,
                      IMM_LAST_CHAR);
            free_c(abc);
            return NULL;
        }

        size_t j = __imm_abc_index(symbols[i]);
        if (abc->symbol_idx[j] != -1) {
            imm_error("alphabet cannot have duplicated symbols");
            free_c(abc);
            return NULL;
        }
        abc->symbol_idx[j] = (int)i;
    }
    abc->symbols = strdup(symbols);

    return abc;
}

struct imm_abc const* imm_abc_clone(struct imm_abc const* abc)
{
    struct imm_abc* nabc = malloc(sizeof(struct imm_abc));

    nabc->symbols = strdup(abc->symbols);
    nabc->length = abc->length;
    memcpy(nabc->symbol_idx, abc->symbol_idx, sizeof(int) * IMM_SYMBOL_IDX_SIZE);
    nabc->any_symbol = abc->any_symbol;

    return nabc;
}

void imm_abc_destroy(struct imm_abc const* abc)
{
    free_c(abc->symbols);
    free_c(abc);
}

enum imm_symbol_type imm_abc_symbol_type(struct imm_abc const* abc, char symbol_id)
{
    if (symbol_id == abc->any_symbol)
        return IMM_SYMBOL_ANY;

    if (imm_abc_has_symbol(abc, symbol_id))
        return IMM_SYMBOL_NORMAL;

    return IMM_SYMBOL_UNKNOWN;
}
