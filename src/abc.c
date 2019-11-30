#include "ascii.h"
#include "hide.h"
#include "imm/imm.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct imm_abc
{
    char const* symbols;
    int         symbol_idx[ASCII_LAST_STD + 1];
    char        any_symbol;
};

static int check_symbols_length(char const* symbols);
static int check_symbol_id(struct imm_abc const* abc, char symbol_id);

struct imm_abc* imm_abc_create(char const* symbols, char any_symbol)
{
    if (check_symbols_length(symbols))
        return NULL;

    struct imm_abc* abc = malloc(sizeof(struct imm_abc));

    abc->symbols = strdup(symbols);
    abc->any_symbol = any_symbol;

    for (int i = 0; i <= ASCII_LAST_STD; ++i)
        abc->symbol_idx[i] = -1;

    char const* ids = abc->symbols;
    int         len = (int)strlen(abc->symbols);
    for (int idx = 0; idx < len; ++idx) {
        if (check_symbol_id(abc, ids[idx])) {
            free((char*)abc->symbols);
            free(abc);
            return NULL;
        }

        abc->symbol_idx[(int)ids[idx]] = idx;
    }

    return abc;
}

void imm_abc_destroy(struct imm_abc* abc)
{
    if (!abc) {
        imm_error("abc should not be NULL");
        return;
    }

    free((char*)abc->symbols);
    free(abc);
}

int imm_abc_length(struct imm_abc const* abc) { return (int)strlen(abc->symbols); }

char const* imm_abc_symbols(struct imm_abc const* abc) { return abc->symbols; }

int imm_abc_has_symbol(struct imm_abc const* abc, char symbol_id)
{
    if (check_symbol_id(abc, symbol_id))
        return 0;
    return abc->symbol_idx[(int)symbol_id] != -1;
}

int imm_abc_symbol_idx(struct imm_abc const* abc, char symbol_id)
{
    if (check_symbol_id(abc, symbol_id))
        return -1;
    return abc->symbol_idx[(int)symbol_id];
}

char imm_abc_symbol_id(struct imm_abc const* abc, int symbol_idx)
{
    return abc->symbols[symbol_idx];
}

static int check_symbols_length(char const* symbols)
{
    if (strlen(symbols) > INT_MAX) {
        imm_error("symbols length is greater than %d", INT_MAX);
        return 1;
    }
    return 0;
}

static int check_symbol_id(struct imm_abc const* abc, char symbol_id)
{
    if (!ascii_is_std(&symbol_id, 1)) {
        imm_error("symbols must be non-extended ASCII characters");
        return 1;
    }
    if (symbol_id == abc->any_symbol) {
        imm_error("symbol cannot be `%c`", abc->any_symbol);
        return 1;
    }
    return 0;
}
