#include "imm.h"
#include "src/imm/hide.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define SYMBOL_ID_MIN 0
#define SYMBOL_ID_MAX 127

struct imm_abc
{
    const char *symbols;
    int symbol_idx[SYMBOL_ID_MAX + 1];
};

HIDE int check_symbols_length(const char *symbols);
HIDE int check_symbol_id(char symbol_id);

struct imm_abc *imm_abc_create(const char *symbols)
{
    if (check_symbols_length(symbols))
        return NULL;

    struct imm_abc *abc = malloc(sizeof(struct imm_abc));

    abc->symbols = strdup(symbols);

    for (int i = 0; i <= SYMBOL_ID_MAX; ++i)
        abc->symbol_idx[i] = -1;

    const char *ids = abc->symbols;
    int len = (int)strlen(abc->symbols);
    for (int idx = 0; idx < len; ++idx) {
        if (check_symbol_id(ids[idx])) {
            free((char *)abc->symbols);
            free(abc);
            return NULL;
        }

        abc->symbol_idx[(int)ids[idx]] = idx;
    }

    return abc;
}

void imm_abc_destroy(struct imm_abc *abc)
{
    if (!abc)
        return;

    free((char *)abc->symbols);
    free(abc);
}

int imm_abc_length(const struct imm_abc *abc) { return (int)strlen(abc->symbols); }

int imm_abc_has_symbol(const struct imm_abc *abc, char symbol_id)
{
    check_symbol_id(symbol_id);
    return abc->symbol_idx[(int)symbol_id] != -1;
}

int imm_abc_symbol_idx(const struct imm_abc *abc, char symbol_id)
{
    if (check_symbol_id(symbol_id))
        return -1;
    return abc->symbol_idx[(int)symbol_id];
}

char imm_abc_symbol_id(const struct imm_abc *abc, int symbol_idx)
{
    return abc->symbols[symbol_idx];
}

int check_symbols_length(const char *symbols)
{
    if (strlen(symbols) > INT_MAX) {
        imm_error("symbols length is greater than %d", INT_MAX);
        return 1;
    }
    return 0;
}

int check_symbol_id(char symbol_id)
{
    if (symbol_id < SYMBOL_ID_MIN || symbol_id > SYMBOL_ID_MAX) {
        imm_error("symbols must be non-extended ASCII characters");
        return 1;
    }
    if (symbol_id == IMM_ANY_SYMBOL) {
        imm_error("symbol cannot be `%c`", IMM_ANY_SYMBOL);
        return 1;
    }
    return 0;
}
