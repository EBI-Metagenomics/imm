#include "ascii.h"
#include "free.h"
#include "imm/imm.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

/**
 * Immutable alphabet.
 *
 * It represents a finite set of symbols and a special any-symbol
 * symbol.
 */
struct imm_abc
{
    char const* symbols;
    int         symbol_idx[ASCII_LAST_STD + 1];
    char        any_symbol;
};

static int check_symbols_length(char const* symbols);
static int check_symbol_id(struct imm_abc const* abc, char symbol_id);

struct imm_abc const* imm_abc_create(char const* symbols, char const any_symbol)
{
    if (check_symbols_length(symbols))
        return NULL;

    if (!ascii_is_std(&any_symbol, 1)) {
        imm_error("any_symbol must be non-extended ASCII characters");
        return NULL;
    }

    struct imm_abc* abc = malloc(sizeof(struct imm_abc));

    abc->symbols = strdup(symbols);
    abc->any_symbol = any_symbol;

    for (int i = 0; i <= ASCII_LAST_STD; ++i)
        abc->symbol_idx[i] = -1;

    char const* ids = abc->symbols;
    int         len = (int)strlen(abc->symbols);
    for (int idx = 0; idx < len; ++idx) {
        if (check_symbol_id(abc, ids[idx])) {
            free_c(abc->symbols);
            free_c(abc);
            return NULL;
        }

        abc->symbol_idx[(size_t)ids[idx]] = idx;
    }

    return abc;
}

struct imm_abc const* imm_abc_clone(struct imm_abc const* abc)
{
    struct imm_abc* nabc = malloc(sizeof(struct imm_abc));

    nabc->symbols = strdup(abc->symbols);
    memcpy(nabc->symbol_idx, abc->symbol_idx, sizeof(int) * ARRAY_LENGTH(abc->symbol_idx));
    nabc->any_symbol = abc->any_symbol;

    return nabc;
}

void imm_abc_destroy(struct imm_abc const* abc)
{
    if (!abc) {
        imm_error("abc should not be NULL");
        return;
    }

    free_c(abc->symbols);
    free_c(abc);
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

char imm_abc_any_symbol(struct imm_abc const* abc) { return abc->any_symbol; }

enum imm_symbol_type imm_abc_symbol_type(struct imm_abc const* abc, char symbol_id)
{
    if (symbol_id == abc->any_symbol)
        return IMM_SYMBOL_ANY;

    if (imm_abc_has_symbol(abc, symbol_id))
        return IMM_SYMBOL_NORMAL;

    return IMM_SYMBOL_UNKNOWN;
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
